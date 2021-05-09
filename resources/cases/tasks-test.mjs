/**
 *
 * expect:
 * one
 * four
 * three
 * two
 */
export function runTaskTest() {
    console.log('one');
    setTimeout(function () {
        console.log('two');
    }, 0);
    Promise.resolve().then(function () {
        console.log('three');
    });
    console.log('four');
}

/**
 *
 * expect:
 * 3456
 */
export function runClearTimeoutTest() {
    let h = setTimeout(() => {
        console.log('1234');
    }, 3000);
    console.log('3456');
    clearTimeout(h);
}
