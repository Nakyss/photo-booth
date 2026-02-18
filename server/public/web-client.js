import PhotoBoothWebSocket from './scripts/webSockerClient.js';
import RoomWebClient from './scripts/roomWebClient.js';

const imageContainer = document.getElementById('imageContainer');
const settingsWindow = document.getElementById('settings-window');

const displayImage = (url) => {
    const img = document.createElement('img');
    img.style.width = "100%";
    img.style.height = "100%";
    img.style.objectFit = "contain";
    img.src = url;
    img.style.opacity = "0";
    img.style.transition = "opacity 0.2s ease";
    img.onload = () => {
        img.style.opacity = "1";
        setTimeout(() => {
            img.style.opacity = "0";
        }, 2500);
        setTimeout(() => {
            img.remove();
        }, 2800);
    }
    imageContainer.appendChild(img);
};


// Initialisation

const roomWebClient = new RoomWebClient();
const photoBoothWS = new PhotoBoothWebSocket(displayImage, roomWebClient.loadRooms());

photoBoothWS.connect();
roomWebClient.ws = photoBoothWS;

const isMobile = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('settings').style.transform = 'translateX(0)';
    if (!isMobile) {
        setTimeout(() => {
            if (settingsWindow.style.display !== 'block') {
                document.getElementById('settings').style = null;
            }
        }, 4000);
    }
});

const setRoomsBtn = document.getElementById('setRoomsBtn');
const roomsInput = document.getElementById('roomsInput');

document.getElementById('settings').addEventListener('click', () => {
    settingsWindow.style.display = settingsWindow.style.display === 'block' ? 'none' : 'block';
    if (settingsWindow.style.display === 'block') {
        roomWebClient.renderRoomsList();
        document.getElementById('settings').style.transform = 'translateX(0)';
    }
    else {
        if (!isMobile){
            document.getElementById('settings').style = null;
        }
    }
});

setRoomsBtn.addEventListener('click', async () => {
    const input = roomsInput.value.trim();
    if (input !== '') roomWebClient.addRoom(input);
    else return;
    
    roomsInput.value = '';
    roomWebClient.renderRoomsList();
});

roomsInput.addEventListener('keyup', (event) => {
    if (event.key === 'Enter') {
        setRoomsBtn.click();
    }
});

if (localStorage.getItem("theme") === "dark") {
    document.body.classList.add("dark");
}