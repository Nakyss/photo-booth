const video = document.getElementById('cameraStream');
const canvas = document.getElementById('photoCanvas');
const photoButton = document.getElementById('photoButton');
const capturesButtons = document.getElementById('capture')
const validationsButtons = document.getElementById('validation-buttons');
const captionInput = document.getElementById('caption');
const ctx = canvas.getContext('2d');
const cameraOption = document.getElementById('cameraOption')
const importBtn = document.getElementById('importBtn');
const importInput = document.getElementById('importInput');
const inverseBtn = document.getElementById('inverseBtn');
let originalImageData = null; // Stocker l'image sans texte

const constraints = {
    video: true
};

const startCamera = async (constraints) =>{
    try {
        video.srcObject = await navigator.mediaDevices.getUserMedia(constraints);
    } catch (err) {
        console.error("Erreur d'accès à la caméra:", err);
        alert("Impossible d'accéder à la caméra.");
    }
}

const capturePhoto = () => {
    const width = video.videoWidth;
    const height = video.videoHeight;
    
    canvas.width = width;
    canvas.height = height;
    ctx.drawImage(video, 0, 0, width, height);
    
    // Sauvegarder l'image originale
    originalImageData = ctx.getImageData(0, 0, width, height);
    
    // Ajouter le texte
    drawCaptionOnCanvas();
    
    video.classList.add('hidden');
    canvas.classList.remove('hidden');
    capturesButtons.classList.add('hidden');
    validationsButtons.classList.remove('hidden');
}

// Convertit le canvas en Blob et envoie en multipart/form-data à /upload
const uploadCapturedPhoto = () => {
    return new Promise((resolve, reject) => {
        // Créer un canvas temporaire haute résolution
        const highResCanvas = document.createElement('canvas');
        const highResCtx = highResCanvas.getContext('2d');
        
        // Calculer le scale pour atteindre 1920x1080 sans dépasser
        const targetWidth = 1920;
        const targetHeight = 1080;
        let scale = 1;
        
        //if (canvas.width < targetWidth && canvas.height < targetHeight) {
            // Calculer le scale nécessaire pour atteindre au moins une des dimensions cibles
            const scaleWidth = targetWidth / canvas.width;
            const scaleHeight = targetHeight / canvas.height;
            scale = Math.min(scaleWidth, scaleHeight);
        //}
        
        highResCanvas.width = Math.floor(canvas.width * scale);
        highResCanvas.height = Math.floor(canvas.height * scale);


        // Améliorer la qualité du rendu
        highResCtx.imageSmoothingEnabled = true;
        highResCtx.imageSmoothingQuality = 'high';
        
        // Redessiner l'image originale en haute résolution
        if (originalImageData) {
            // Créer un canvas temporaire pour l'image originale
            const tempCanvas = document.createElement('canvas');
            tempCanvas.width = canvas.width;
            tempCanvas.height = canvas.height;
            const tempCtx = tempCanvas.getContext('2d');
            tempCtx.putImageData(originalImageData, 0, 0);
            
            // Dessiner en haute résolution
            highResCtx.drawImage(tempCanvas, 0, 0, highResCanvas.width, highResCanvas.height);
        }
        
        // Redessiner le texte en haute résolution
        const text = captionInput.value.trim();
        if (text) {
            const width = highResCanvas.width;
            const height = highResCanvas.height;
            const fontSize = Math.floor(height / 12);
            
            highResCtx.font = `bold ${fontSize}px Arial, sans-serif`;
            highResCtx.textAlign = 'center';
            highResCtx.textBaseline = 'bottom';
            
            const padding = fontSize /2;
            const maxWidth = width - (padding * 2);
            
            // Découper le texte en lignes si nécessaire
            const words = text.split(' ');
            const lines = [];
            let currentLine = words[0];
            
            for (let i = 1; i < words.length; i++) {
                const word = words[i];
                const testLine = currentLine + ' ' + word;
                const metrics = highResCtx.measureText(testLine);
                
                if (metrics.width > maxWidth) {
                    lines.push(currentLine);
                    currentLine = word;
                } else {
                    currentLine = testLine;
                }
            }
            lines.push(currentLine);
            
            // Position de départ (en bas de l'image)
            const lineHeight = fontSize * 1.2;
            let y = height - padding;
            
            // Dessiner chaque ligne
            for (let i = lines.length - 1; i >= 0; i--) {
                const x = width / 2;
                
                // Contour du texte
                highResCtx.strokeStyle = 'black';
                highResCtx.lineWidth = 5;
                highResCtx.lineJoin = 'round';
                highResCtx.strokeText(lines[i], x, y);
                
                // Texte blanc
                highResCtx.fillStyle = 'white';
                highResCtx.fillText(lines[i], x, y);
                
                y -= lineHeight;
            }
        }
        
        // Convertir le canvas haute résolution en blob
        highResCanvas.toBlob(async (blob) => {
            if (!blob) return reject(new Error('Impossible de créer le blob'));

            // Afficher les informations de l'image générée
            console.log('📸 Image générée:');
            console.log(`  - Dimensions: ${highResCanvas.width}x${highResCanvas.height}px`);
            console.log(`  - Taille: ${(blob.size / 1024 ).toFixed(2)} KB / 15 MB`);
            console.log(`  - Scale appliqué: ${scale.toFixed(2)}x`);
            

            const form = new FormData();
            form.append('image', blob, `photo-${Date.now()}.png`);
            form.append('message', '');
            
            try {
                const res = await fetch('/upload', {
                    method: 'POST',
                    body: form
                });
                if (!res.ok) {
                    const err = await res.text();
                    return reject(new Error(err || 'Erreur lors de l\'upload'));
                }
                const json = await res.json();
                resolve(json);
            } catch (e) {
                reject(e);
            }
        }, 'image/png');
    });
}

photoButton.addEventListener('click', async () => {
    capturePhoto();
});

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


document.getElementById('valid').addEventListener('click', async () => {
    try {
        const notification = showNotification();

        canvas.classList.add('hidden');
        video.classList.remove('hidden');
        validationsButtons.classList.add('hidden');
        capturesButtons.classList.remove('hidden');

        const result = await uploadCapturedPhoto();
        console.log('Upload réussi:', result);
        hideNotification(notification);
        // Remettre la caméra et cacher le canvas

    } catch (err) {
        console.error('Upload échoué:', err);
        alert('Échec de l\'upload.');
        // remettre la caméra pour retenter
        canvas.classList.add('hidden');
        video.classList.remove('hidden');
        validationsButtons.classList.add('hidden');
        capturesButtons.classList.remove('hidden');
    }
});

// Fonction pour dessiner le caption sur le canvas
const drawCaptionOnCanvas = () => {
    const text = captionInput.value.trim();
    if (!text) return;
    
    const width = canvas.width;
    const height = canvas.height;
    
    // Configuration du style du texte
    const fontSize = Math.floor(height / 12); // Taille proportionnelle à l'image
    ctx.font = `bold ${fontSize}px Arial`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';
    
    const padding = fontSize / 2 ;
    const maxWidth = width - (padding * 2);
    
    // Découper le texte en lignes si nécessaire
    const words = text.split(' ');
    const lines = [];
    let currentLine = words[0];
    
    for (let i = 1; i < words.length; i++) {
        const word = words[i];
        const testLine = currentLine + ' ' + word;
        const metrics = ctx.measureText(testLine);
        
        if (metrics.width > maxWidth) {
            lines.push(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    lines.push(currentLine);
    
    // Position de départ (en bas de l'image)
    const lineHeight = fontSize * 1.2;
    const totalHeight = lines.length * lineHeight;
    let y = height - padding;
    
    // Dessiner chaque ligne
    for (let i = lines.length - 1; i >= 0; i--) {
        const x = width / 2;
        
        // Contour noir
        ctx.strokeStyle = 'black';
        ctx.lineWidth = 2;
        ctx.lineJoin = 'round';
        ctx.miterLimit = 2;
        ctx.strokeText(lines[i], x, y);
        
        // Texte blanc
        ctx.fillStyle = 'white';
        ctx.fillText(lines[i], x, y);
        
        y -= lineHeight;
    }
}


document.getElementById('delete').addEventListener('click', () => {
    canvas.classList.add('hidden');
    video.classList.remove('hidden');
    validationsButtons.classList.add('hidden');
    capturesButtons.classList.remove('hidden');
});

captionInput.addEventListener('input', () => {
    //captionPreview.textContent = captionInput.value;
    // Redessiner l'image avec le nouveau texte
    if (!canvas.classList.contains('hidden') && originalImageData) {
        ctx.putImageData(originalImageData, 0, 0);
        drawCaptionOnCanvas();
    }
});

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

if (localStorage.getItem("theme") === "dark") {
    document.body.classList.add("dark");
    document.getElementById("checkbox").checked = true;
}

const checkbox = document.getElementById("checkbox")
checkbox.addEventListener("change", () => {
    if (checkbox.checked) {
        localStorage.setItem("theme", "dark");
        document.body.classList.add("dark");
    } else {
        localStorage.setItem("theme", "light");
        document.body.classList.remove("dark");
    }
})


const getCameraSelection = async () => {
    await navigator.mediaDevices.getUserMedia({video: true});
    const devices = await navigator.mediaDevices.enumerateDevices();
    const videoDevices = devices.filter(device => device.kind === 'videoinput');
    const options = videoDevices.map(videoDevice => {
        return `<option value="${videoDevice.deviceId}">${videoDevice.label}</option>`;
    });
    cameraOption.innerHTML = options.join('');
    if (videoDevices.length > 0) {
        const updateConstraints = {
            ...constraints,
            video : {
                deviceId: {
                    exact: videoDevices[0].deviceId
                }
            }
        };
        startCamera(updateConstraints);
    }
};


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
    const updateConstraints = {
        ...constraints,
        video: {
            deviceId: {
                exact: event.target.value
            }
        }
    };

    startCamera(updateConstraints);
});

const displayImportedImage = dataURL => {
    const img = new Image();
    img.onload = () => {
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0);
        canvas.classList.remove('hidden');
        video.classList.add('hidden');
        validationsButtons.classList.remove('hidden');
        capturesButtons.classList.add('hidden');
        originalImageData = ctx.getImageData(0, 0, img.width, img.height);
        drawCaptionOnCanvas();
    };
    img.src = dataURL;
}


const onFileSelected = event => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => displayImportedImage(e.target.result);
    reader.readAsDataURL(file);
}



importBtn.addEventListener('click', () => {
    importInput.value = '';
    importInput.click();
});

importInput.addEventListener('change', (e) => onFileSelected(e));


const handlePasteImage = (event) => {
    const items = event.clipboardData?.items;
    if (!items) return;

    for (const item of items) {
        if (item.type.startsWith("image/")) {
            const file = item.getAsFile();
            const reader = new FileReader();

            reader.onload = (e) => {
                displayImportedImage(e.target.result);
            };

            reader.readAsDataURL(file);

            event.preventDefault();
            return;
        }
    }
};

document.addEventListener("paste", handlePasteImage);
