import express from 'express';
import cors from 'cors';
import http from 'http';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import fs from 'fs';
import multer from 'multer';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);


export default class AppServer {
    constructor(userManager) {
        this.app = express();
        this.server = http.createServer(this.app);
        this.socketManager = null;
        this.userManager = userManager;

        // TTL en ms (15 secondes)
        this.uploadTtlMs = 15 * 1000;
        this._scheduledDeletions = new Map();

        // Middleware
        this.app.use(cors());
        this.app.use(express.json());

        // Serve the public directory
        this.app.use(express.static(join(__dirname, '../../public')));

        // Ensure uploads directory exists
        this.uploadsDir = join(__dirname, '../../public/uploads');
        const uploadsDir = join(__dirname, '../../public/uploads');
        if (!fs.existsSync(uploadsDir)) {
            fs.mkdirSync(uploadsDir, { recursive: true });
        }

        this._cleanupAndScheduleExisting();

        // Multer storage
        const storage = multer.diskStorage({
            destination: uploadsDir,
            filename: (req, file, cb) => {
                // simple filename sanitization
                const safeName = file.originalname.replace(/\s+/g, '-').replace(/[^a-zA-Z0-9.\-_]/g, '');
                cb(null, `${Date.now()}-${safeName}`);
            }
        });

        const upload = multer({
            storage,
            limits: { fileSize: 3 * 1024 * 1024 } // 3MB limit
        });

        // Routes
        this.app.get('/', (req, res) => {
            res.sendFile(join(__dirname, '../../public/index.html'));
        });

        this.app.get('/installation_guide', (req, res) => {
            res.sendFile(join(__dirname, '../../public/installation_guide.html'));
        });
        
        this.app.get('/client', (req, res) => {
            res.sendFile(join(__dirname, '../../public/web-client.html'));
        });

        // POST /upload : accepte un champ `image` (multipart/form-data)
        this.app.post('/upload', upload.single('image'), (req, res) => {
            if (!req.file) {
                return res.status(400).json({ error: 'No file uploaded' });
            }

            const publicPath = `/uploads/${req.file.filename}`;
            const fullPath = join(this.uploadsDir, req.file.filename);

            // Schedule deletion after TTL
            this._scheduleDeletion(req.file.filename, fullPath, this.uploadTtlMs);

            if (this.socketManager) {
                this.socketManager.broadcastPhoto(publicPath, req.body.message || '', JSON.parse(req.body.rooms) || []);
            }

            return res.json({ path: publicPath });
        });

        this.app.get('/users', (req, res) => {
            const users = this.userManager.getUserCount();
            res.json(users);
        });
    }

    setSocketManager(socketManager) {
        this.socketManager = socketManager;
    }


    /**
     * Start listening on a port
     * @param {number} port - port number to bind the server to
     */
    start(port = 3000) {
        this.server.listen(port, () => {
            console.log(`Server running on http://localhost:${port}`);
        });
    }

    _scheduleDeletion(filename, fullPath, delayMs) {
        const publicPath = `/uploads/${filename}`;

        // If already scheduled, clear and reschedule
        if (this._scheduledDeletions.has(filename)) {
            clearTimeout(this._scheduledDeletions.get(filename));
        }

        const tid = setTimeout(() => {
            fs.unlink(fullPath, (err) => {
                if (err) {
                    // fichier peut déjà avoir été supprimé
                    console.warn(`Erreur suppression fichier ${publicPath}:`, err.message);
                } else {
                    console.log(`File deleted: ${publicPath}`);
                }
                this._scheduledDeletions.delete(filename);
            });
        }, delayMs);

        this._scheduledDeletions.set(filename, tid);
    }

    _cleanupAndScheduleExisting() {
        try {
            const files = fs.readdirSync(this.uploadsDir);
            const now = Date.now();

            files.forEach((file) => {
                try {
                    const p = join(this.uploadsDir, file);
                    const stat = fs.statSync(p);
                    // utiliser mtime pour gérer la durée depuis la dernière modification
                    const age = now - stat.mtimeMs;

                    if (age >= this.uploadTtlMs) {
                        // trop vieux -> supprimer immédiatement
                        fs.unlinkSync(p);
                        console.log(`Old files deleted at start: ${p}`);
                    } else {
                        // planifier suppression restante
                        const remaining = this.uploadTtlMs - age;
                        this._scheduleDeletion(file, p, remaining);
                        console.log(`Suppression planifiée pour ${p} dans ${Math.round(remaining)}ms`);
                    }
                } catch (e) {
                    console.warn(`Erreur traitement fichier ${file}:`, e.message);
                }
            });
        } catch (e) {
            console.warn('Erreur lors du nettoyage des uploads au démarrage:', e.message);
        }
    }
}
