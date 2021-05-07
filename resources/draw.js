console.log(123123);

print(version);
print(inner);
print(inner.version);
print(JSON.stringify((inner)));
print(inner.log);
inner.log('inner.log');
print(console.log);
print(console);


//
// for (let key in console) {
//     print(' ' + key + " " + console[key]);
// }


console1.log(123);
console1.log('hello ' + `v${8}.`);

Promise.resolve().then(() => {
    print('a');
    Promise.resolve().then(() => {
        print('d');
    });
});
setTimeout(() => {
    print('c');
}, 0);
setTimeout(() => {
    Promise.resolve().then(() => {
        print('g');
    });
    setTimeout(() => {
        print('f');
    }, 0);
    print('e');
}, 1);


console.log('b');



print('b');
setTimeout(() => {
    print('c');
}, 0);

function mainLogic() {
    // setTimeout Test

    console.log('');
    console.log('setTimeout Tests start:');

    setTimeout(() => {
        console.log('timeout 1 triggered.');
        setTimeout(() => {
            console.log('timeout 2 triggered.');
            setTimeout(() => {
                try {
                    ee();
                } catch (e) {
                    console.log(e);
                }
                console.log('timeout 3 triggered.');
            }, 1000);
            setTimeout(() => {
                console.log('timeout 4 triggered.');
            }, 5000);
        }, 2000);
    }, 1000);

// render test

    let y = 10;

    function renderFrame() {
        glClear();
        glText(
            'V8 + gl ' + Math.random() + '.',
            10,
            y,
            30
        );
        y += 10;
        y = y > 470 ? 10 : y;
        if (!glfwTick()) {
            return;
        }
        // loop draw
        setTimeout(renderFrame, 100);
    }

    // renderFrame();
}

// mainLogic();


// base script
function main() {
    runLoop();
    do {
        runLoop();
    } while (isLoopAlive());
    closeLoop();
}

main();