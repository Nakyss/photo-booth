import AppServer from './server/AppServer.js';
import SocketManager from './server/SocketManager.js';
import UserManager from './server/UserManager.js';

const appServer = new AppServer();
const userManager = new UserManager();
const socketManager = new SocketManager(appServer.server, userManager);

appServer.setSocketManager(socketManager);

socketManager.initialize();


const port = 3000;
appServer.start(port);

