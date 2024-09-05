class Memory {
    constructor() {
        this.memory = new Uint8Array(4096);
    }

    read(address) {
        return this.memory[address];
    }

    write(address, value) {
        this.memory[address] = value;
    }
}

export default Memory;