import AppServer from './manager/AppServer.js';
import SocketManager from './manager/SocketManager.js';
import UserManager from './manager/UserManager.js';

const userManager = new UserManager();
const appServer = new AppServer(userManager);
const socketManager = new SocketManager(appServer.server, userManager);

appServer.setSocketManager(socketManager);

socketManager.initialize();

const port = 59875;
appServer.start(port);

