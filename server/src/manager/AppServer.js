import express from 'express';
import cors from 'cors';
import http from 'http';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import uploadService from "../service/uploadService.js";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);


export default class AppServer {
    constructor(userManager) {
        this.app = express();
        this.server = http.createServer(this.app);
        this.socketManager = null;
        this.userManager = userManager;
        this.uploadService = new uploadService();

        // Middleware
        this.app.use(cors());
        this.app.use(express.json());


        // Routes
        this.app.get('/', (req, res) => {
            res.sendFile(join(__dirname, '../../public/index.html'));
        });

        this.app.get('/installation_guide', (req, res) => {
            res.sendFile(join(__dirname, '../../public/installation_guide.html'));
        });
        this.app.get('/rooms_guide', (req, res) => {
            res.sendFile(join(__dirname, '../../public/rooms_guide.html'));
        });
        this.app.get('/legal', (req, res) => {
            res.sendFile(join(__dirname, '../../public/legal.html'));
        });
        
        this.app.get('/client', (req, res) => {
            res.sendFile(join(__dirname, '../../public/web-client.html'));
        });

        // POST /upload : accepte un champ `image` (multipart/form-data)
        this.app.post('/upload', this.uploadService.getUpload().single('image'), (req, res) => {
            if (!req.file) {
                return res.status(400).json({ error: 'No file uploaded' });
            }

            const publicPath = this.uploadService.registerImage(req.file.filename);

            if (this.socketManager) {
                this.socketManager.broadcastPhoto(publicPath, req.body.message || '', JSON.parse(req.body.rooms) || []);
            }

            return res.json({ path: publicPath });
        });

        this.app.get('/users', (req, res) => {
            const users = this.userManager.getUserCount();
            res.json(users);
        });

        this.app.get('/users/:room', (req, res) => {
            const { room } = req.params;
            res.json(this.userManager.getUserCountByRoom(room))
        })

        // Serve the public directory
        this.app.use(express.static(join(__dirname, '../../public')));
    }

    setSocketManager(socketManager) {
        this.socketManager = socketManager;
    }


    /**
     * Start listening on a port
     * @param {number} port - port number to bind the manager to
     */
    start(port = 3000) {
        this.server.listen(port, () => {
            console.log(`Server running on http://127.0.0.1:${port}`);
        });
    }
}
