const CPU = require('./cpu');


describe('CPU', () => {
  let cpu;

  beforeEach(() => {
    cpu = new CPU();
  });

  test('should initialize with correct defaults', () => {
    expect(cpu.registers.length).toBe(16);
    expect(cpu.registers.every(reg => reg === 0)).toBe(true);
    expect(cpu.I).toBe(0);
    expect(cpu.PC).toBe(0x200);
    expect(cpu.stack).toEqual([]);
    expect(cpu.SP).toBe(0);
    expect(cpu.delayTimer).toBe(0);
    expect(cpu.soundTimer).toBe(0);
    expect(cpu.display.every(pixel => pixel === 0)).toBe(true);
    expect(cpu.keys.every(key => key === 0)).toBe(true);
  });

  test('reset should set all values to initial state', () => {
    cpu.registers[0] = 1;
    cpu.I = 100;
    cpu.PC = 0x300;
    cpu.stack.push(0x400);
    cpu.SP = 1;
    cpu.delayTimer = 10;
    cpu.soundTimer = 5;

    cpu.reset();

    expect(cpu.registers.length).toBe(16);
    expect(cpu.registers.every(reg => reg === 0)).toBe(true);
    expect(cpu.I).toBe(0);
    expect(cpu.PC).toBe(0x200);
    expect(cpu.stack).toEqual([]);
    expect(cpu.SP).toBe(0);
    expect(cpu.delayTimer).toBe(0);
    expect(cpu.soundTimer).toBe(0);
    expect(cpu.display.every(pixel => pixel === 0)).toBe(true);
    expect(cpu.keys.every(key => key === 0)).toBe(true);
  });

  test('fetch should return correct opcode and increment PC', () => {
    cpu.memory.write(0x200, 0xAB);
    cpu.memory.write(0x201, 0xCD);

    const opcode = cpu.fetch();

    expect(opcode).toBe(0xABCD);
    expect(cpu.PC).toBe(0x202);
  });

  test('step should fetch and execute an opcode', () => {
    const mockExecute = jest.spyOn(cpu, 'execute');
    cpu.memory.write(0x200, 0xAB);
    cpu.memory.write(0x201, 0xCD);

    cpu.step();

    expect(mockExecute).toHaveBeenCalledWith(0xABCD);
    expect(cpu.PC).toBe(0x202);

    mockExecute.mockRestore();
  });


  describe('Instructions', () => {
    test('0x00E0 - should clear the display', () => {
      // Fill the display with 1s
      cpu.display.fill(1);

      // Execute the clear display instruction
      cpu.execute(0x00E0);

      // check if all pixels are now 0
      expect(cpu.display.every(pixel => pixel === 0)).toBe(true);
    });

    test('1NNN - should jump to address NNN', () => {
      // Execute the jump instruction
      cpu.execute(0x1ABC);

      // Check if program counter (PC) is set to the correct address
      expect(cpu.PC).toBe(0xABC);
    });

    test('6XNN - should set register VX to NN', () => {
      cpu.execute(0x6AFF); // Set VA tto 0xFF
      expect(cpu.registers[0xA]).toBe(0xFF);
    });

    test('7XNN - should add NN to register VX', () => {
      cpu.registers[0x1] = 0x10;
      cpu.execute(0x7120); // Add 0x20 to V1
      expect(cpu.registers[0x1]).toBe(0x30);
    });

    test('7XNN - should handle overflow correctly', () => {
      cpu.registers[0x1] = 0xFF;
      cpu.execute(0x7102); // Add 2 to V1 (should overflow)
      expect(cpu.registers[0x1]).toBe(0x01);
    });

    test('ANNN - should set index register I to NNN', () => {
      cpu.execute(0xA123) // Set I to 0x123
      expect(cpu.I).toBe(0x123);
    });

    test('FX07 - should set VX to the value of delay timer', () => {
      cpu.delayTimer = 5;
      cpu.execute(0xF007);
      expect(cpu.registers[0]).toBe(5);
    });

    test('FX15 - should set the delay timer to VX', () => {
      cpu.registers[0] = 10;
      cpu.execute(0xF015);
      expect(cpu.delayTimer).toBe(10);
    });

    test('FX18 - should set the sound timer to VX', () => {
      cpu.registers[0] = 15;
      cpu.execute(0xF018);
      expect(cpu.soundTimer).toBe(15);
    });

    test('FX1E - should add VX to I', () => {
      cpu.registers[0] = 25;
      cpu.I = 100;
      cpu.execute(0xF01E);
      expect(cpu.I).toBe(125);
    });

    test('FX1E - should handle overflow and wrap I around 16-bits', () => {
      cpu.registers[0] = 1;
      cpu.I = 0xFFFF;
      cpu.execute(0xF01E);
      expect(cpu.I).toBe(0) // (1 + 0xFFFF) & 0xFFFF = 0
    });


    describe('8XY* operations', () => {
      test('8XY0 should set VX to the value of VY', () => {
        cpu.registers[2] = 0xAB;
        cpu.execute(0x8120);
        expect(cpu.registers[1]).toBe(0xAB);
      });

      test('8XY1 should set VX to VX | VY', () => {
        cpu.registers[1] = 0x0F;
        cpu.registers[2] = 0xF0;
        cpu.execute(0x8121);
        expect(cpu.registers[1]).toBe(0xFF);
      });

      test('8XY2 should set VX to VX & VY', () => {
        cpu.registers[1] = 0xF0;
        cpu.registers[2] = 0x0F;
        cpu.execute(0x8122);
        expect(cpu.registers[1]).toBe(0x00);
      });

      test('8XY3 should set VX to VX ^ VY', () => {
        cpu.registers[1] = 0xFF;
        cpu.registers[2] = 0x0F;
        cpu.execute(0x8123);
        expect(cpu.registers[1]).toBe(0xF0);
      });

      test('8XY4 should add VY to VX and set VF if carry occurs', () => {
        cpu.registers[1] = 0xFF;
        cpu.registers[2] = 0x01;
        cpu.execute(0x8124);
        expect(cpu.registers[1]).toBe(0x00); // Wrapped value
        expect(cpu.registers[0xF]).toBe(1); // Carry flag
      });

      test('8XY5 should subtract VY from VX and set VF if no borrow occurs', () => {
        cpu.registers[1] = 0x05;
        cpu.registers[2] = 0x03;
        cpu.execute(0x8125);
        expect(cpu.registers[1]).toBe(0x02);
        expect(cpu.registers[0xF]).toBe(1); // No borrow
      });

      test('8XY5 should subtract VY from VX and set VF if borrow occurs', () => {
        cpu.registers[1] = 0x03;
        cpu.registers[2] = 0x05;
        cpu.execute(0x8125);
        expect(cpu.registers[1]).toBe(0xFE); // Wrapped value
        expect(cpu.registers[0xF]).toBe(0); // Borrow occurred
      });

      test('8XY6 should store LSB of VX in VF and shift VX right by 1', () => {
        cpu.registers[1] = 0x03; // Binary: 00000011
        cpu.execute(0x8106);
        expect(cpu.registers[1]).toBe(0x01); // Binary: 00000001
        expect(cpu.registers[0xF]).toBe(1);  // LSB was 1
      });

      test('8XY7 should set VX to VY - VX and set VF if no borrow occurs', () => {
        cpu.registers[1] = 0x03;
        cpu.registers[2] = 0x05;
        cpu.execute(0x8127);
        expect(cpu.registers[1]).toBe(0x02);
        expect(cpu.registers[0xF]).toBe(1); // No borrow
      });

      test('8XY7 should set VX to VY - VX and set VF if borrow occurs', () => {
        cpu.registers[1] = 0x05;
        cpu.registers[2] = 0x03;
        cpu.execute(0x8127);
        expect(cpu.registers[1]).toBe(0xFE); // Wrapped value
        expect(cpu.registers[0xF]).toBe(0); // Borrow occurred
      });

      test('8XYE should store MSB of VX in VF and shift VX left by 1', () => {
        cpu.registers[1] = 0x80; // Binary: 10000000
        cpu.execute(0x810E);
        expect(cpu.registers[1]).toBe(0x00); // Binary: 00000000
        expect(cpu.registers[0xF]).toBe(1);  // MSB was 1
      });
    });


    describe('DXYN - Draw sprite', () => {
      beforeEach(() => {
        // Setup memory with a simple sprite
        cpu.I = 0x300;
        cpu.memory.write(0x300, 0b11110000);
        cpu.memory.write(0x301, 0b10010000);
        cpu.memory.write(0x302, 0b11110000);
      });

      test('should draw sprite correctly', () => {
        cpu.registers[0] = 0;
        cpu.registers[1] = 0;
        cpu.execute(0xD013); // Draw 3-byte sprite at (0,0)

        const display = cpu.getDisplay();
        expect(display[0]).toBe(1);
        expect(display[1]).toBe(1);
        expect(display[2]).toBe(1);
        expect(display[2]).toBe(1);
        expect(display[64]).toBe(1);
        expect(display[67]).toBe(1);
        expect(display[128]).toBe(1);
        expect(display[129]).toBe(1);
        expect(display[130]).toBe(1);
        expect(display[131]).toBe(1);
      });

      test('should set VF to 1 on collision', () => {
        // Draw sprite once
        cpu.registers[0] = 0;
        cpu.registers[1] = 0;
        cpu.execute(0xD013);

        // Draw sprite again at the same position
        cpu.execute(0xD013);

        expect(cpu.registers[0xF]).toBe(1);
      });

      test('should wrap sprites correctly', () => {
        cpu.registers[0] = 62; // Near right edge
        cpu.registers[1] = 30; // Near bottom edge
        cpu.execute(0xD013); // Draw 3-bytes sprite

        const display = cpu.getDisplay();

        // Check wrapping on right edge (row 1)
        expect(display[30 * 64 + 62]).toBe(1); // 62, 30
        expect(display[30 * 64 + 63]).toBe(1); // 63, 30
        expect(display[30 * 64 + 0]).toBe(1);  // 0, 30
        expect(display[30 * 64 + 1]).toBe(1);  // 1, 30

        // Check wrapping on right edge (row 2)
        expect(display[31 * 64 + 62]).toBe(1); // 62, 31
        expect(display[31 * 64 + 1]).toBe(1);  // 1, 31

        // Check wrapping on bottom and right edge (row 3)
        expect(display[0 * 64 + 62]).toBe(1);  // 62, 0
        expect(display[0 * 64 + 63]).toBe(1);  // 63, 0
        expect(display[0 * 64 + 0]).toBe(1);   // 0, 0
        expect(display[0 * 64 + 1]).toBe(1);   // 1, 0

        // Check that pixels that should be off are off
        expect(display[30 * 64 + 2]).toBe(0);  // 2, 30
        expect(display[31 * 64 + 63]).toBe(0); // 63, 31
        expect(display[31 * 64 + 0]).toBe(0);  // 0, 31

      });
    });

  });


  describe('Input handling', () => {
    test('setKey should update key state', () => {
      cpu.setKey(0xA, true);
      expect(cpu.keys[0xA]).toBe(1);
      cpu.setKey(0xA, false);
      expect(cpu.keys[0xA]).toBe(0);
    });

    test('EX9E should skip next instruction if key VX is pressed', () => {
      cpu.registers[0] = 0xA;
      cpu.setKey(0xA, true);
      cpu.PC = 0x200;
      cpu.execute(0xE09E);
      expect(cpu.PC).toBe(0x202);
    });

    test('EX9E should not skip next instruction if key VX is not pressed', () => {
      cpu.registers[0] = 0xA;
      cpu.setKey(0xA, false);
      cpu.PC = 0x200;
      cpu.execute(0xE09E);
      expect(cpu.PC).toBe(0x200);
    });

    test('EXA1 should skip next instruction if key VX is not pressed', () => {
      cpu.registers[0] = 0xA;
      cpu.setKey(0xA, false);
      cpu.PC = 0x200;
      cpu.execute(0xE0A1);
      expect(cpu.PC).toBe(0x202);
    });

    test('EXA1 should not skip next instruction if key VX is pressed', () => {
      cpu.registers[0] = 0xA;
      cpu.setKey(0xA, true);
      cpu.PC = 0x200;
      cpu.execute(0xE0A1);
      expect(cpu.PC).toBe(0x200);
    });

    test('FX0A should wait for key press and store key value', () => {
      cpu.PC = 0x200;
      cpu.execute(0xF00A); // wait for key press and store in V0
      expect(cpu.PC).toBe(0x1FE); // PC should decrease to repeat instruction

      cpu.setKey(0x5, true);
      cpu.execute(0xF00A); // Now key is pressed
      expect(cpu.registers[0]).toBe(0x5);
      expect(cpu.PC).toBe(0x1FE);
    });
  })
})
