import { canvas } from "./htmlElement.js";


const showNotification = () => {
    const toolbox = document.querySelector('.toolbox');
    const notification = document.createElement('div');
    notification.classList.add('notification');

    const imageDataURL = canvas.toDataURL('image/png');

    notification.innerHTML = `
        <img src="${imageDataURL}" alt="Envoi en cours">
        <p>Envoie en cours</p>
    `;
    toolbox.appendChild(notification);
    
    return notification;
}

const hideNotification = (notification) => {
    notification.querySelector('p').textContent = 'Envoi terminé';
    setTimeout(() => {
        notification.remove();
    }, 2000);
}

export { showNotification, hideNotification };