class RoomWebClient {
    constructor() {
        this.rooms = [];
        this.ws = null;
    }

    removeRoom = (room) => {
        this.rooms = this.rooms.filter(r => r !== room);
        localStorage.setItem("clientRooms", JSON.stringify(this.rooms));
        this.ws.updateRooms(this.rooms);
    }

    addRoom = (room) => {
        if (!this.rooms.includes(room)) {
            this.rooms.push(room);
            localStorage.setItem("clientRooms", JSON.stringify(this.rooms));
            this.ws.updateRooms(this.rooms);
        }
    }

    loadRooms = () => {
        const storedRooms = localStorage.getItem("clientRooms");
        if (storedRooms) {
            this.rooms = JSON.parse(storedRooms);
        }
        return this.rooms;
    }

    renderRoomsList = () => {
        roomList.innerHTML = '';
        this.rooms.forEach(room => {
            const el = document.createElement('li');
            
            const button = document.createElement('button');
            button.innerHTML = '<i class="fa-solid fa-trash"></i>';
            button.style.marginLeft = '5px';
            button.addEventListener('click', () => {
                roomList.removeChild(el);
                this.removeRoom(room);
            });
            button.title = "Supprimer la salle";
            
            const text = document.createElement('span');
            text.textContent = room;
            
            el.appendChild(text);
            el.appendChild(button);
            roomList.appendChild(el);
        });
    }
}

export default RoomWebClient;