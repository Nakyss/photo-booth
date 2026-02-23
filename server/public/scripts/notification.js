import { canvas } from "./htmlElement.js";


const showNotification = () => {
    const toolbox = document.querySelector('.toolbox');
    const notification = document.createElement('div');
    notification.classList.add('notification');

    const imageDataURL = canvas.toDataURL('image/png');

    notification.innerHTML = `
        <img src="${imageDataURL}" alt="Envoi en cours">
        <div class="notification-content">
            <p>Envoi en cours</p>
            <div class="progress-bar">
                <div class="progress-fill"></div>
            </div>
        </div>
    `;
    toolbox.appendChild(notification);
    
    return notification;
}

const updateProgress = (notification, percentage) => {
    const progressFill = notification.querySelector('.progress-fill');
    if (progressFill) {
        progressFill.style.width = `${percentage}%`;
    }
}

const hideNotification = (notification) => {
    notification.querySelector('p').textContent = 'Envoi terminé';
    const progressFill = notification.querySelector('.progress-fill');
    if (progressFill) {
        progressFill.style.width = '100%';
        progressFill.style.backgroundColor = '#0d41cf';
    }
    setTimeout(() => {
        notification.remove();
    }, 2000);
}

export { showNotification, hideNotification, updateProgress };