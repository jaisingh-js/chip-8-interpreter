import Memory from "./memory.js";

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
    this.display = new Array(64 * 32).fill(0);
    this.keys = new Array(16).fill(0);
  }

  reset() {
    this.registers.fill(0);
    this.I = 0;
    this.PC = 0x200;
    this.stack = [];
    this.SP = 0;
    this.delayTimer = 0;
    this.soundTimer = 0;
    this.display.fill(0);
    this.keys.fill(0);
  }

  fetch() {
    const highByte = this.memory.read(this.PC);
    const lowByte = this.memory.read(this.PC + 1);
    this.PC += 2;
    return (highByte << 8) | lowByte;
  }

  setKey(key, isPressed) {
    this.keys[key] = isPressed ? 1 : 0;
  }

  execute(opcode) {
    switch (opcode & 0xF000) {
      case 0x0000:
        if (opcode === 0x00E0) {
          //clear the display
          this.display.fill(0);
        }
        break;
      case 0x1000:
        //Jump to address NNN
        this.PC = opcode & 0x0FFF;
        break;
      case 0x6000:
        // Set register VX to NN
        const x = (opcode & 0x0F00) >> 8;
        const nn = opcode & 0x00FF;
        this.registers[x] = nn;
        break;
      case 0x7000:
        // Add NN to register VX
        const x7 = (opcode & 0x0F00) >> 8;
        const nn7 = opcode & 0x00FF;
        this.registers[x7] += nn7;

        // Handle overflow
        this.registers[x7] &= 0xFF;
        break;
      case 0xA000:
        // set index register I to NNN
        this.I = opcode & 0x0FFF;
        break;
      case 0xD000: {
        // Draw sprite
        const x = this.registers[(opcode & 0x0F00) >> 8];
        const y = this.registers[(opcode & 0x00F0) >> 4];
        const height = opcode & 0x000F;

        this.registers[0xF] = 0; // Reset VF

        for (let row = 0; row < height; row++) {
          const spriteByte = this.memory.read(this.I + row);

          for (let col = 0; col < 8; col++) {
            if ((spriteByte & (0x80 >> col)) !== 0) {
              //Calculate wrapped coordinates
              const xCoord = (x + col) & 63;
              const yCoord = (y + row) & 31;
              const index = yCoord * 64 + xCoord;

              if (this.display[index] === 1) {
                this.registers[0xF] = 1; // Collision
              }
              this.display[index] ^= 1;
            }
          }
        }

        break;
      }
      case 0xE000:
        switch (opcode & 0x00FF) {
          case 0x009E: // EX9E: Skip next instruction if key VX is pressed
            if (this.keys[this.registers[(opcode & 0x0F00) >> 8]] !== 0) {
              this.PC += 2;
            }
            break;
          case 0x00A1: // EXA1: Skip next instruction if key VX is not pressed
            if (this.keys[this.registers[(opcode & 0x0F00) >> 8]] === 0) {
              this.PC += 2;
            }
        }
        break;
      case 0xF000:
        switch (opcode & 0x00FF) {
          case 0x000A: { // FX0A: Wait for key press, store the value of key in VX
            const x = (opcode & 0x0F00) >> 8;
            const keyPress = this.keys.findIndex(key => key !== 0);
            if (keyPress !== -1) {
              this.registers[x] = keyPress;
            }
            else {
              this.PC -= 2; // If no key is pressed, repeat this instruction
            }
            break;
          }

          case 0x0007: { // FX07: Set VX to the value of the delay timer
            const x = (opcode & 0x0F00) >> 8;
            this.registers[x] = this.delayTimer;
            break;
          }

          case 0x0015: { // FX15: Set delay timer to VX
            const x = (opcode & 0x0F00) >> 8;
            this.delayTimer = this.registers[x];
            break;
          }

          case 0x0018: { // FX18: Set sound timer to VX
            const x = (opcode & 0x0F00) >> 8;
            this.soundTimer = this.registers[x];
            break;
          }

          case 0x001E: { // FX1E: Add VX to I
            const x = (opcode & 0x0F00) >> 8;
            this.I += this.registers[x];
            this.I &= 0xFFFF // Ensure I stays within 16-bit
            break;
          }
        }
        break;

      case 0x8000:
        switch (opcode & 0x000F) {
          case 0x0000: // 8XY0: Set VX to the value of VY
            this.registers[(opcode & 0x0F00) >> 8] = this.registers[(opcode & 0x00F0) >> 4];
            break;
          case 0x0001: // 8XY1: Set VX to VX | VY
            this.registers[(opcode & 0x0F00) >> 8] |= this.registers[(opcode & 0x00F0) >> 4];
            break;
          case 0x0002: // 8XY2: Set VX to VX & VY
            this.registers[(opcode & 0x0F00) >> 8] &= this.registers[(opcode & 0x00F0) >> 4];
            break;
          case 0x0003: // 8XY3: Set VX to VX ^ VY
            this.registers[(opcode & 0x0F00) >> 8] ^= this.registers[(opcode & 0x00F0) >> 4];
            break;
          case 0x0004: { // 8XY4: Add VY to VX, set VF if carry
            const x = (opcode & 0x0F00) >> 8;
            const y = (opcode & 0x00F0) >> 4;
            const sum = this.registers[x] + this.registers[y];
            this.registers[0xF] = sum > 0xFF ? 1 : 0;
            this.registers[x] = sum & 0xFF;
            break;
          }
          case 0x0005: { // 8XY5: Subtract VY from VX, set VF if no borrow
            const x = (opcode & 0x0F00) >> 8;
            const y = (opcode & 0x00F0) >> 4;
            this.registers[0xF] = this.registers[x] > this.registers[y] ? 1 : 0;
            this.registers[x] = (this.registers[x] - this.registers[y]) & 0xFF;
            break;
          }
          case 0x0006: { // 8XY6: Store LSB of VX in VF, then shift VX right by 1
            const x = (opcode & 0x0F00) >> 8;
            this.registers[0xF] = this.registers[x] & 0x1;
            this.registers[x] >>= 1;
            break;
          }
          case 0x0007: { // 8XY7: Set VX to VY - VX, set VF if no borrow
            const x = (opcode & 0x0F00) >> 8;
            const y = (opcode & 0x00F0) >> 4;
            this.registers[0xF] = this.registers[y] > this.registers[x] ? 1 : 0;
            this.registers[x] = (this.registers[y] - this.registers[x]) & 0xFF;
            break;
          }
          case 0x000E: { // 8XYE: Store MSB of VX in VF, then shift VX left by 1
            const x = (opcode & 0x0F00) >> 8;
            this.registers[0xF] = (this.registers[x] & 0x80) >> 7;
            this.registers[x] = (this.registers[x] << 1) & 0xFF;
            break;
          }
        }
        break;
      default:
        console.log(`Unknown opcode: ${opcode.toString(16)}`);
    }
  }

  getDisplay() {
    return this.display;
  }

  step() {
    const opcode = this.fetch();
    this.execute(opcode);
  }
}

export { CPU };