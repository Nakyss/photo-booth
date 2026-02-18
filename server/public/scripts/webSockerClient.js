class PhotoBoothWebSocket {
    constructor(onImageReceived, rooms) {
        this.onImageReceived = onImageReceived;
        this.reconnectDelay = 2000;
        this.ws = null;
        this.shouldReconnect = true;
        this.rooms = rooms;
    }
    
    connect() {
        const protocol = location.protocol === 'https:' ? 'wss' : 'ws';
        const wsUrl = `${protocol}://${location.host}/ws`;
        
        this.ws = new WebSocket(wsUrl);
        
        this.ws.addEventListener('open', () => this.onOpen(wsUrl));
        this.ws.addEventListener('message', (evt) => this.onMessage(evt));
        this.ws.addEventListener('close', (e) => this.onClose(e));
        this.ws.addEventListener('error', (err) => this.onError(err));
    }
    
    onOpen(wsUrl) {
        console.log('WebSocket connecté:', wsUrl);
        this.updateRooms(this.rooms);
    }
    
    onMessage(evt) {
        try {
            const msg = JSON.parse(evt.data);
            
            if (msg.event === 'photo:broadcast' && msg.data?.image) {
                console.log("Une Image a été envoyée");
                console.log(msg.data.image, msg.data.message);
                this.onImageReceived(msg.data.image);
            } else {
                console.debug('Message WS inconnu:', msg);
            }
        } catch (e) {
            console.warn('Impossible de parser le message WS:', e, evt.data);
        }
    }
    
    onClose(e) {
        console.warn('WebSocket fermé', e);
        
        if (this.shouldReconnect) {
            console.log(`Tentative de reconnexion dans ${this.reconnectDelay}ms`);
            setTimeout(() => this.connect(), this.reconnectDelay);
        }
    }
    
    onError(err) {
        console.error('Erreur WebSocket:', err);
        this.ws?.close();
    }
    
    disconnect() {
        this.shouldReconnect = false;
        this.ws?.close();
    }
    
    updateRooms(rooms) {
        if (this.ws?.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify({ event: 'room:changed', data: { rooms: rooms } }));
        }
    }
}

export default PhotoBoothWebSocket;