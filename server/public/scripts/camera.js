import { drawCaptionOnCanvas, setCanvaSize, ctx, setOriginalImage } from "./canva.js";
import { getCaption } from "./inputs.js";
import { video, cameraOption, timerBtn } from "./htmlElement.js";
import { hideVideo } from "./documentAction.js";

const constraints = {
    video: true
};

/**
 * Start the camera with the given constraints, containing the device ID.
 * @param {Object} constraints 
 */
const startCamera = async (constraints) =>{
    try {
        video.srcObject = await navigator.mediaDevices.getUserMedia(constraints);
    } catch (err) {
        console.error("Erreur d'accès à la caméra:", err);
        alert("Impossible d'accéder à la caméra.");
    }
}

/**
 * Capture a photo from the video stream and draw it on the canvas
 */
const capturePhoto = () => {
    const canvaSize = setCanvaSize(video.videoWidth, video.videoHeight);

    ctx.drawImage(video, 0, 0, canvaSize.width, canvaSize.height);
    
    // Sauvegarder l'image originale
    setOriginalImage(ctx.getImageData(0, 0, canvaSize.width, canvaSize.height));
    
    // Ajouter le texte
    drawCaptionOnCanvas(getCaption());
    
    hideVideo();
}

/**
 * Get the list of available cameras and populate the camera selection dropdown.
 */
const getCameraSelection = async () => {
    await navigator.mediaDevices.getUserMedia({video: true});
    const devices = await navigator.mediaDevices.enumerateDevices();
    const videoDevices = devices.filter(device => device.kind === 'videoinput');
    const options = videoDevices.map(videoDevice => {
        return `<option value="${videoDevice.deviceId}">${videoDevice.label}</option>`;
    });
    cameraOption.innerHTML = options.join('');
    if (videoDevices.length > 0) {
        const updateConstraints = setDeviceConstraints(videoDevices[0].deviceId);
        startCamera(updateConstraints);
    }
};

/**
 * Handle the countdown timer before capturing a photo
 * @param {int} duration 
 */
const handleTimer = (duration) => {
    let countdown = duration;
    timerBtn.disabled = true;
    const originalIcon = timerBtn.innerHTML;

    timerBtn.innerHTML = `<span style="font-size: 1.3em;">${countdown}</span>`;
    countdown--;

    const interval = setInterval(() => {
        if (countdown > 0) {
            timerBtn.innerHTML = `<span style="font-size: 1.3em;">${countdown}</span>`;
            countdown--;
        } else {
            clearInterval(interval);
            timerBtn.innerHTML = originalIcon;
            timerBtn.disabled = false;
            capturePhoto();
        }
    }, 1000);
};

/**
 * Update constraints to use the specified device ID
 * @param {*} deviceId the device ID of the selected camera
 * @returns {Object} Updated constraints with the specified device ID
 */
const setDeviceConstraints = (deviceId) => {
    return {
        ...constraints,
        video : {
            deviceId: {
                exact: deviceId
            }
        }
    };
}

export { startCamera, capturePhoto, getCameraSelection, handleTimer, setDeviceConstraints };