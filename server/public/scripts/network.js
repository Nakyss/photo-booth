import { getCaption } from "./inputs.js";
import { getActiveRooms } from "./room.js";

const getForm = (image) => {
    const form = new FormData();
    form.append('image', image, `photo-${Date.now()}.png`);
    form.append('message', getCaption());
    form.append('rooms', JSON.stringify(getActiveRooms()));

    return form;
}

const sendImage = (form, onProgress) => {
    return new Promise((resolve, reject) => {
        try {
            const xhr = new XMLHttpRequest();
            
            // Gérer la progression de l'upload
            xhr.upload.addEventListener('progress', (e) => {
                if (e.lengthComputable && onProgress) {
                    const percentComplete = (e.loaded / e.total) * 100;
                    onProgress(percentComplete);
                }
            });
            
            xhr.addEventListener('load', () => {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        const json = JSON.parse(xhr.responseText);
                        resolve(json);
                    } catch {
                        reject(new Error('Erreur de parsing de la réponse'));
                    }
                } else {
                    reject(new Error(xhr.responseText || 'Erreur lors de l\'upload'));
                }
            });
            
            xhr.addEventListener('error', () => {
                reject(new Error('Erreur réseau'));
            });
            
            xhr.open('POST', '/upload');
            xhr.send(form);
            
        } catch (e) {
            reject(e);
        }
    });
};

export {getForm, sendImage}