const Memory = require('./memory');

describe('Memory', () => {
    let memory;

    beforeEach(() => {
        memory = new Memory();
    });

    test('Should initialize with 4096 bytes', () => {
        expect(memory.memory.length).toBe(4096);
    });

    test('Should write and read a byte correctly', () => {
        memory.write(0x200, 0xAB);
        expect(memory.read(0x200)).toBe(0xAB);
    });

    test('Should handle out-of-bounds read', () => {
        expect(memory.read(4096)).toBeUndefined();
    });

    test('Should handle out-of-bounds write', () => {
        memory.write(4096, 0xAB);
        expect(memory.read(4096)).toBeUndefined();
    });

})