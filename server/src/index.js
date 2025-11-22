import AppServer from './server/AppServer.js';
import SocketManager from './server/SocketManager.js';
import UserManager from './server/UserManager.js';

const userManager = new UserManager();
const appServer = new AppServer(userManager);
const socketManager = new SocketManager(appServer.server, userManager);

appServer.setSocketManager(socketManager);

socketManager.initialize();


const port = 59875;
appServer.start(port);

