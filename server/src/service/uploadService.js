import fs from 'fs';
import { dirname, join } from 'path';
import {fileURLToPath} from "url";
import multer from "multer";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

export default class uploadService{
    constructor() {
        this.uploadTtlMs = 15 * 1000;
        this._scheduledDeletions = new Map();

        // Ensure uploads directory exists
        this.uploadsDir = join(__dirname, '../../public/uploads');
        if (!fs.existsSync(this.uploadsDir)) {
            fs.mkdirSync(this.uploadsDir, { recursive: true });
        }

        this._cleanupAndScheduleExisting();

        // Multer storage
        const storage = multer.diskStorage({
            destination: this.uploadsDir,
            filename: (req, file, cb) => {
                const safeName = file.originalname.replace(/\s+/g, '-').replace(/[^a-zA-Z0-9.\-_]/g, '');
                cb(null, `${Date.now()}-${safeName}`);
            }
        });

        const upload = multer({
            storage,
            limits: { fileSize: 5 * 1024 * 1024 } // 5MB limit
        });

        this.getUpload = () => {
            return upload;
        }
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

    registerImage = (filename) => {
        const publicPath = `/uploads/${filename}`;
        const fullPath = join(this.uploadsDir, filename);
        // Schedule deletion after TTL
        this._scheduleDeletion(filename, fullPath, this.uploadTtlMs);
        return publicPath;
    }
}