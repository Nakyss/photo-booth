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
        if (!res.ok) throw new Error('Erreur lors de la récupération du nombre d\'utilisateurs');
        const userCount = await res.json();
        const el = document.getElementById('userCount')
        el.textContent = userCount;
        if (userCount > 0) {
            el.style.color = 'green';
        } else {
            el.style.color = 'red';
        }
    } catch (err) {
        console.error(err);
    }
};

export { hideVideo, showVideo, updateUserCount };