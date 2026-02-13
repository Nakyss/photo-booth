import { roomList } from "./htmlElement.js";

let rooms = [];

const removeRoom = (room) => {
    rooms = rooms.filter(r => r !== room);
    localStorage.setItem("rooms", JSON.stringify(rooms));
}

const addRoom = (room) => {
    if (!rooms.includes(room)) {
        rooms.push(room);
    }
    localStorage.setItem("rooms", JSON.stringify(rooms));
}

const loadRooms = () => {
    const storedRooms = localStorage.getItem("rooms");
    if (storedRooms) {
        rooms = JSON.parse(storedRooms);
    }
}

const renderRoomsList = () => {
    roomList.innerHTML = '';
    rooms.forEach(room => {
        const el = document.createElement('li');

        const button = document.createElement('button');
        button.innerHTML = '<i class="fa-solid fa-trash"></i>';
        button.addEventListener('click', () => {
            roomList.removeChild(el);
            removeRoom(room);
        });

        const text = document.createElement('span');
        text.textContent = ' '+room;

        el.appendChild(button);
        el.appendChild(text);
        roomList.appendChild(el);
    });
}


export { rooms, removeRoom, addRoom, loadRooms, renderRoomsList };