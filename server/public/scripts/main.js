import { canvas, photoButton, cameraOption, importBtn, importInput, inverseBtn, timerBtn, darkModeToggle, captionInput, video } from "./htmlElement.js";
import { handleTimer, capturePhoto, setDeviceConstraints, getCameraSelection, startCamera } from "./camera.js";
import { showVideo, updateUserCount } from "./documentAction.js";
import { hideNotification, showNotification } from "./notification.js";
import { originalImage, drawCaptionOnCanvas, uploadCapturedPhoto, ctx } from "./canva.js";
import { onFileSelected, handlePasteImage, handleDragOver, handleDragLeave, handleDrop } from "./imported.js";
import { getCaption } from "./inputs.js";


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



document.getElementById('reload').addEventListener('click', () => {
    updateUserCount();
    document.querySelector('#reload i').classList.add('rotate');
    setTimeout(() => {
        document.querySelector('#reload i').classList.remove('rotate');
    }, 1000);
});

document.addEventListener('DOMContentLoaded', () => {
    updateUserCount();
    getCameraSelection();
});

inverseBtn.addEventListener('click', () => {
    video.classList.toggle('inversed');
});

cameraOption.addEventListener('change', (event) => {
    const updateConstraints = setDeviceConstraints(event.target.value);
    startCamera(updateConstraints);
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

