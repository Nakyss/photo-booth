// SocketManager.js
import { WebSocketServer } from 'ws';

export default class SocketManager {
    constructor(server, userManager) {
        this.wss = new WebSocketServer({ server });
        this.userManager = userManager;
    }

    initialize() {
        this.wss.on('connection', (ws) => {
            const id = Date.now().toString(36) + Math.random().toString(36).slice(2);
            console.log('A user connected:', id);

            const newUser = this.userManager.addUser(id);
            ws._id = id;

            // prévenir les autres
            this.broadcastExcept(ws, 'user_connected', newUser);

            ws.on('message', (msg) => {
                try {
                    const { event, data } = JSON.parse(msg);
                    if (event === 'disconnect') {
                        this.handleDisconnect(ws);
                    }
                } catch (e) {
                    console.warn('Invalid message:', msg.toString());
                }
            });

            ws.on('close', () => this.handleDisconnect(ws));
        });
    }

    handleDisconnect(ws) {
        if (!ws._id) return;
        console.log('User disconnected:', ws._id);
        this.userManager.removeUser(ws._id);
        this.broadcastExcept(ws, 'user_disconnected', { id: ws._id });
    }

    broadcastPhoto(img, message = '') {
        if (!img) return;
        const payload = JSON.stringify({
            event: 'photo:broadcast',
            data: { image: img, message }
        });
        this.broadcastAll(payload);
        console.log('New picture:', img);
    }

    broadcastExcept(exclude, event, data) {
        const payload = JSON.stringify({ event, data });
        this.wss.clients.forEach(c => {
            if (c !== exclude && c.readyState === 1) c.send(payload);
        });
    }

    broadcastAll(payload) {
        this.wss.clients.forEach(c => {
            if (c.readyState === 1) c.send(payload);
        });
    }
}
