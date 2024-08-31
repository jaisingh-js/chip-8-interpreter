const Memory = require('./memory');

class CPU {
    constructor() {
        this.memory = new Memory();
        this.registers = new Uint8Array(16); //V0 to VF
        this.I = 0; //Index register
        this.PC = 0x200; //Program Counter
        this.stack = [];
        this.SP = 0; //Stack Pointer
        this.delayTimer = 0;
        this.soundTimer = 0;
    }

    reset() {
        this.registers.fill(0);
        this.I = 0;
        this.PC = 0x200;
        this.stack = [];
        this.SP = 0;
        this.delayTimer = 0;
        this.soundTimer = 0;
    }

    fetch() {
        const highByte = this.memory.read(this.PC);
        const lowByte = this.memory.read(this.PC + 1);
        this.PC += 2;
        return (highByte << 8) | lowByte;
    }

    execute(opcode) {
        //TODO later
        console.log(`Executing opcode: ${opcode.toString(16)}`);
    }

    step() {
        const opcode = this.fetch();
        this.execute(opcode);
    }
}

module.exports = CPU;