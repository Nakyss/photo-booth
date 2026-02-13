import { roomList } from "./htmlElement.js";

let rooms = [];

const removeRoom = (room) => {
    rooms = rooms.filter(r => r.name !== room);
    localStorage.setItem("rooms", JSON.stringify(rooms));
}

const addRoom = (room) => {
    if (!rooms.map(r => r.name).includes(name)) {
        rooms.push({name: room, active: true});
    }
    localStorage.setItem("rooms", JSON.stringify(rooms));
}

const toggleRoom = async (room) => {
    rooms.find(r => r === room).active = !room.active;
    localStorage.setItem("rooms", JSON.stringify(rooms));
    await updateRoomsCount();
}

const loadRooms = () => {
    const storedRooms = localStorage.getItem("rooms");
    if (storedRooms) {
        rooms = JSON.parse(storedRooms);
        if (rooms.length !== 0) {
            if (typeof rooms[0] === "string") {
                rooms = rooms.map(r => ({name: r, active: true}));
            }
        }
    }
}

const renderRoomsList = async () => {
    roomList.innerHTML = '';
    rooms.forEach(room => {
        const el = document.createElement('li');

        const buttons = document.createElement('div');
        buttons.classList.add('room-buttons');
        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = room.active;
        checkbox.title = "Activer/désactiver la salle";
        checkbox.addEventListener('change', () => {
            toggleRoom(room);
        });
        buttons.appendChild(checkbox);

        const button = document.createElement('button');
        button.innerHTML = '<i class="fa-solid fa-trash"></i>';
        button.addEventListener('click', () => {
            roomList.removeChild(el);
            removeRoom(room.name);
        });
        button.title = "Supprimer la salle";
        buttons.appendChild(button);

        const roomInfo = document.createElement('div');
        roomInfo.className = 'room-info';
        const text = document.createElement('span');
        text.textContent = room.name;

        const countEl = document.createElement('span');
        countEl.textContent = ` (?)`;
        countEl.style.fontWeight = 'normal';
        countEl.style.color = 'grey';
        countEl.className = 'countEl';

        roomInfo.appendChild(text)
        roomInfo.appendChild(countEl);

        el.appendChild(roomInfo);
        el.appendChild(buttons);
        roomList.appendChild(el);
    });
    await updateRoomsCount();
}

const updateRoomsCount = async () => {
    const allRooms = document.querySelectorAll('#roomList li');
    for (const room of allRooms) {
        const roomName = room.querySelector('span').textContent;
        try {
            const res = await fetch('/users/' + roomName);
            if (!res.ok) console.error("Erreur lors de la récupération du nombre d'utilisateurs pour la salle " + roomName);
            const userCount = await res.json();
            const el = room.querySelector('.countEl');
            el.textContent = ` (${userCount})`;
            if (userCount > 0) {
                el.style.color = 'green';
            } else {
                el.style.color = 'red';
            }
        } catch (err) {
            console.error(err);
        }
    }
}

const getActiveRooms = () => {
    return rooms.filter(r => r.active).map(r => r.name);
}

export { removeRoom, addRoom, loadRooms, renderRoomsList, updateRoomsCount, getActiveRooms };