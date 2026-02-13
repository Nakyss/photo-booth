import { canvas, photoButton, cameraOption, importBtn, importInput, inverseBtn, timerBtn, darkModeToggle, captionInput, video, roomsInput, setRoomsBtn } from "./scripts/htmlElement.js";
import { handleTimer, capturePhoto, setDeviceConstraints, getCameraSelection, startCamera } from "./scripts/camera.js";
import { showVideo, updateUserCount } from "./scripts/documentAction.js";
import { hideNotification, showNotification } from "./scripts/notification.js";
import { originalImage, drawCaptionOnCanvas, uploadCapturedPhoto, ctx } from "./scripts/canva.js";
import { onFileSelected, handlePasteImage, handleDragOver, handleDragLeave, handleDrop } from "./scripts/imported.js";
import { getCaption } from "./scripts/inputs.js";
import {addRoom, loadRooms, renderRoomsList, updateRoomsCount} from "./scripts/room.js";


// capture button
photoButton.addEventListener('click', async () => {
    capturePhoto();
});

// Timer button
timerBtn.addEventListener('click', () => {
    handleTimer(3);
});


// WHen validation button is clicked
document.getElementById('valid').addEventListener('click', async () => {
    const notification = showNotification();
    showVideo();
    try {
        const result = await uploadCapturedPhoto();
        console.log('Upload réussi:', result);
        hideNotification(notification);

    } catch (err) {
        console.error('Upload échoué:', err);
        alert('Échec de l\'upload.');
        if (notification) hideNotification(notification);
    }
});


document.getElementById('delete').addEventListener('click', () => {
    showVideo();
});

captionInput.addEventListener('input', () => {
    if (!canvas.classList.contains('hidden') && originalImage) {
        ctx.putImageData(originalImage, 0, 0);
        drawCaptionOnCanvas(getCaption());
    }
});


if (localStorage.getItem("theme") === "dark") {
    document.body.classList.add("dark");
    document.getElementById("checkbox").checked = true;
}


darkModeToggle.addEventListener("change", () => {
    if (darkModeToggle.checked) {
        localStorage.setItem("theme", "dark");
        document.body.classList.add("dark");
    } else {
        localStorage.setItem("theme", "light");
        document.body.classList.remove("dark");
    }
})



document.getElementById('reload').addEventListener('click', async () => {
    await updateUserCount();
    document.querySelector('#reload i').classList.add('rotate');
    await updateRoomsCount();
    setTimeout(() => {
        document.querySelector('#reload i').classList.remove('rotate');
    }, 1000);
});

document.addEventListener('DOMContentLoaded', async () => {
    loadRooms();
    await renderRoomsList();
    await updateUserCount();
    await getCameraSelection();
});

inverseBtn.addEventListener('click', () => {
    video.classList.toggle('inversed');
});

cameraOption.addEventListener('change', async (event) => {
    const updateConstraints = setDeviceConstraints(event.target.value);
    await startCamera(updateConstraints);
});


importBtn.addEventListener('click', () => {
    importInput.value = '';
    importInput.click();
});

importInput.addEventListener('change', (e) => onFileSelected(e));


document.addEventListener("paste", handlePasteImage);
document.addEventListener("dragover", handleDragOver);
document.addEventListener("dragleave", handleDragLeave);
document.addEventListener("drop", handleDrop);



setRoomsBtn.addEventListener('click', async () => {
    const input = roomsInput.value.trim();
    if (input !== '') addRoom(input);
    else return;

    roomsInput.value = '';
    await renderRoomsList();
});

roomsInput.addEventListener('keyup', (event) => {
    if (event.key === 'Enter') {
        setRoomsBtn.click();
    }
});