const canvas = document.getElementById('photoCanvas');
const video = document.getElementById('cameraStream');
const photoButton = document.getElementById('photoButton');
const capturesButtons = document.getElementById('capture')
const validationsButtons = document.getElementById('validation-buttons');
const cameraOption = document.getElementById('cameraOption')
const importBtn = document.getElementById('importBtn');
const importInput = document.getElementById('importInput');
const inverseBtn = document.getElementById('inverseBtn');
const timerBtn = document.getElementById('timerBtn');
const captionInput = document.getElementById('caption');
const darkModeToggle = document.getElementById("checkbox")

export { 
    canvas, 
    video, 
    photoButton, 
    capturesButtons, 
    validationsButtons, 
    cameraOption, 
    importBtn, 
    importInput, 
    inverseBtn, 
    timerBtn, 
    captionInput,
    darkModeToggle
};