// SocketManager.js
import { WebSocketServer } from 'ws';

export default class SocketManager {
    constructor(server, userManager) {
        this.wss = new WebSocketServer({ server });
        this.userManager = userManager;
        
        // Intervalle pour vérifier les clients morts (toutes les 30s)
        this.heartbeatInterval = null;
    }

    initialize() {
        // Démarrer la surveillance des connexions
        this.startHeartbeat();

        this.wss.on('connection', (ws) => {
            const id = Date.now().toString(36) + Math.random().toString(36).slice(2);
            console.log('A user connected:', id);

            // 1. Initialiser l'état "vivant" du nouveau client
            ws.isAlive = true;
            ws._id = id;

            // 2. Écouter le "pong" du client (réponse à nos pings serveur)
            ws.on('pong', () => {
                ws.isAlive = true;
            });

            // Ajout au User Manager
            const newUser = this.userManager.addUser(id);

            // Prévenir les autres
            this.broadcastExcept(ws, 'user_connected', newUser);

            ws.on('message', (msg) => {
                try {
                    // Si on reçoit un message, le client est forcément vivant
                    ws.isAlive = true; 
                    
                    const { event, data } = JSON.parse(msg);
                    if (event === 'disconnect') {
                        this.handleDisconnect(ws);
                    }
                } catch (e) {
                    console.warn('Invalid message:', msg.toString());
                }
            });

            // Note: ws gère déjà le ping entrant venant de Qt et répond pong automatiquement.
            // On peut ajouter ceci pour le debug si besoin :
            
            /*ws.on('ping', () => {
                console.log('Ping reçu du client (Qt), Pong envoyé auto.');
                ws.isAlive = true;
            });*/
            

            ws.on('close', () => this.handleDisconnect(ws));
        });

        this.wss.on('close', () => {
            clearInterval(this.heartbeatInterval);
        });
    }

    // --- NOUVELLE FONCTION : Gestion du Ping/Pong Serveur ---
    startHeartbeat() {
        this.heartbeatInterval = setInterval(() => {
            this.wss.clients.forEach((ws) => {
                // Si le client n'a pas répondu au dernier ping (isAlive est faux)
                if (ws.isAlive === false) {
                    console.log('Client inactif (timeout), fermeture de la connexion:', ws._id);
                    return ws.terminate(); // On coupe brutalement
                }

                // On marque le client comme "suspect" (false) et on lui envoie un ping
                ws.isAlive = false;
                ws.ping(); // Envoie un ping standard WebSocket
            });
        }, 30000); // Vérification toutes les 30 secondes
    }

    handleDisconnect(ws) {
        if (!ws._id) return;
        
        // Vérification pour éviter les doubles suppressions
        if(this.userManager.getUser(ws._id)) {
            console.log('User disconnected:', ws._id);
            this.userManager.removeUser(ws._id);
            this.broadcastExcept(ws, 'user_disconnected', { id: ws._id });
        }
    }

    broadcastPhoto(img, message = '') {
        if (!img) return;
        const payload = JSON.stringify({
            event: 'photo:broadcast',
            data: { image: img, message }
        });
        this.broadcastAll(payload);
        console.log('New picture sent to clients');
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