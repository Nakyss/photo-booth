export default class UserManager {
    constructor() {
        this.connectedUsers = new Map();
    }

    addUser(userId) {
        const user = {
            id: userId,
            rooms: []
        };

        this.connectedUsers.set(userId, user);
        return user;
    }

    getUser(userId) {
        return this.connectedUsers.get(userId);
    }

    removeUser(userId) {
        this.connectedUsers.delete(userId);
    }

    setUserRooms(userId, rooms) {
        const user = this.getUser(userId);
        if (user) {
            user.rooms = rooms;
        }
        return user;
    }

    getAllUsers() {
        return Array.from(this.connectedUsers.values());
    }

    getUserCount() {
        return this.connectedUsers.size;
    }
}
