import { video, canvas, capturesButtons, validationsButtons } from "./htmlElement.js";

/**
 * Hide the video element and show the render with appropriate buttons
 */
const hideVideo = () => {
    video.classList.add('hidden');
    canvas.classList.remove('hidden');
    capturesButtons.classList.add('hidden');
    validationsButtons.classList.remove('hidden');
};


/**
 * Show the video element and hide the render with appropriate buttons
 */
const showVideo = () => {
    video.classList.remove('hidden');
    canvas.classList.add('hidden');
    capturesButtons.classList.remove('hidden');
    validationsButtons.classList.add('hidden');
};

/**
 * Update the user count displayed in the UI
 */
const updateUserCount = async () => {
    try {
        const res = await fetch('/users');
        if (!res.ok) console.error("Erreur lors de la récupération du nombre d'utilisateurs");
        const userCount = await res.json();
        const el = document.getElementById('userCount')
        el.textContent = userCount;
        if (userCount > 0) {
            el.style.color = 'var(--primary-color)';
        } else {
            el.style.color = 'red';
        }
    } catch (err) {
        console.error(err);
    }
};

/**
 * Set up a timer overlay on the video element
 */
const setTimerOverlay = () => {
    const timerOverlay = document.createElement('div');
    timerOverlay.classList.add('timerOverlay');
    timerOverlay.style.height = video.getBoundingClientRect().height + 'px';
    timerOverlay.style.width = video.getBoundingClientRect().width + 'px';
    video.parentElement.insertBefore(timerOverlay, video);
    return timerOverlay;
}

export { hideVideo, showVideo, updateUserCount, setTimerOverlay };