// CryptRover Web - Test Harness
// Simple assertion-based test runner for pure logic (no DOM)

// Test result tracking
let passed = 0;
let failed = 0;

// ANSI colors for terminal output
const colors = {
  green: '\x1b[32m',
  red: '\x1b[31m',
  yellow: '\x1b[33m',
  cyan: '\x1b[36m',
  reset: '\x1b[0m',
};

// Assert helper - throws on failure for fail-fast behavior
export function assert(condition: boolean, message: string): void {
  if (condition) {
    passed++;
    console.log(`  ${colors.green}✓${colors.reset} ${message}`);
  } else {
    failed++;
    console.log(`  ${colors.red}✗${colors.reset} ${message}`);
    throw new Error(`Assertion failed: ${message}`);
  }
}

// Assert equality helper
export function assertEqual<T>(actual: T, expected: T, message: string): void {
  const eq = JSON.stringify(actual) === JSON.stringify(expected);
  if (eq) {
    passed++;
    console.log(`  ${colors.green}✓${colors.reset} ${message}`);
  } else {
    failed++;
    console.log(`  ${colors.red}✗${colors.reset} ${message}`);
    console.log(`    Expected: ${JSON.stringify(expected)}`);
    console.log(`    Actual:   ${JSON.stringify(actual)}`);
    throw new Error(`Assertion failed: ${message}`);
  }
}

// Suite helper for grouping tests
export function suite(name: string, fn: () => void): void {
  console.log(`\n${colors.cyan}${name}${colors.reset}`);
  try {
    fn();
  } catch (e) {
    // Test already logged, just stop this suite
  }
}

// Import utilities for testing
import { setSeed, randInt, dist2, inRange, bresenham, min, max, clamp } from '../utils.js';

// Run all tests
function runTests(): void {
  console.log(`${colors.yellow}CryptRover Web - Test Harness${colors.reset}`);
  console.log('================================\n');

  // Set deterministic seed for reproducible tests
  const testSeed = 1234;
  setSeed(testSeed);
  console.log(`Using seed: ${testSeed}`);

  // Utils tests
  suite('Utils: Math Helpers', () => {
    assertEqual(min(3, 5), 3, 'min(3, 5) = 3');
    assertEqual(min(5, 3), 3, 'min(5, 3) = 3');
    assertEqual(max(3, 5), 5, 'max(3, 5) = 5');
    assertEqual(max(5, 3), 5, 'max(5, 3) = 5');
    assertEqual(clamp(5, 0, 10), 5, 'clamp(5, 0, 10) = 5');
    assertEqual(clamp(-5, 0, 10), 0, 'clamp(-5, 0, 10) = 0');
    assertEqual(clamp(15, 0, 10), 10, 'clamp(15, 0, 10) = 10');
  });

  suite('Utils: dist2 (squared distance)', () => {
    assertEqual(dist2(0, 0, 3, 4), 25, 'dist2(0,0,3,4) = 25 (3-4-5 triangle)');
    assertEqual(dist2(0, 0, 0, 0), 0, 'dist2(0,0,0,0) = 0');
    assertEqual(dist2(1, 1, 4, 5), 25, 'dist2(1,1,4,5) = 25');
    assertEqual(dist2(0, 0, 1, 1), 2, 'dist2(0,0,1,1) = 2 (diagonal)');
  });

  suite('Utils: inRange', () => {
    assert(inRange(5, 5, 3, 5, 5), 'center is in range');
    assert(inRange(5, 5, 3, 5, 7), 'point at radius edge is in range');
    assert(inRange(5, 5, 3, 8, 5), 'point at radius edge on x is in range');
    assert(!inRange(5, 5, 3, 5, 9), 'point beyond radius is not in range');
    assert(!inRange(5, 5, 3, 10, 10), 'distant point is not in range');
    assert(inRange(0, 0, 5, 3, 4), '3-4-5 triangle is within radius 5');
  });

  suite('Utils: bresenham (line drawing)', () => {
    const line1 = bresenham(0, 0, 4, 0);
    assertEqual(line1.length, 5, 'horizontal line has 5 points');
    assertEqual(line1[0], { x: 0, y: 0 }, 'starts at origin');
    assertEqual(line1[4], { x: 4, y: 0 }, 'ends at (4,0)');

    const line2 = bresenham(0, 0, 0, 3);
    assertEqual(line2.length, 4, 'vertical line has 4 points');
    assertEqual(line2[3], { x: 0, y: 3 }, 'ends at (0,3)');

    const line3 = bresenham(0, 0, 3, 3);
    assertEqual(line3.length, 4, 'diagonal line has 4 points');
    assert(line3.some(p => p.x === 1 && p.y === 1), 'diagonal passes through (1,1)');
    assert(line3.some(p => p.x === 2 && p.y === 2), 'diagonal passes through (2,2)');
  });

  suite('Utils: randInt (seeded RNG)', () => {
    // Reset seed for deterministic test
    setSeed(1234);
    const r1 = randInt(0, 100);
    const r2 = randInt(0, 100);
    const r3 = randInt(0, 100);

    assert(r1 >= 0 && r1 <= 100, `randInt in range: ${r1}`);
    assert(r2 >= 0 && r2 <= 100, `randInt in range: ${r2}`);
    assert(r3 >= 0 && r3 <= 100, `randInt in range: ${r3}`);

    // Verify determinism - same seed produces same sequence
    setSeed(1234);
    assertEqual(randInt(0, 100), r1, 'same seed produces same first value');
    assertEqual(randInt(0, 100), r2, 'same seed produces same second value');
    assertEqual(randInt(0, 100), r3, 'same seed produces same third value');

    // Verify bounds with small range
    setSeed(5678);
    for (let i = 0; i < 20; i++) {
      const r = randInt(5, 10);
      assert(r >= 5 && r <= 10, `randInt(5,10) in bounds: ${r}`);
    }
    assert(true, 'all 20 randInt calls in bounds');
  });

  // Print summary
  console.log('\n================================');
  console.log(`${colors.yellow}Results:${colors.reset}`);
  console.log(`  ${colors.green}Passed: ${passed}${colors.reset}`);
  if (failed > 0) {
    console.log(`  ${colors.red}Failed: ${failed}${colors.reset}`);
    process.exit(1);
  } else {
    console.log(`\n${colors.green}All tests passed!${colors.reset}`);
  }
}

// Run tests
runTests();
