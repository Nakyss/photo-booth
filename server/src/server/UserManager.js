export default class UserManager {
    constructor() {
        this.connectedUsers = new Map();
    }

    addUser(userId) {
        const user = {
            id: userId,
        };

        this.connectedUsers.set(userId, user);
        return user;
    }


    removeUser(userId) {
        this.connectedUsers.delete(userId);
    }


    getAllUsers() {
        return Array.from(this.connectedUsers.values());
    }
}
