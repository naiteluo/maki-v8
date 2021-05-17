export function canvasApiTest() {
    let canvas = new Canvas();
    console.log("canvas:", canvas);
    console.log(canvas.getMsg());
    canvas.setMsg("canvas hi.");
    console.log(canvas.getMsg());
    console.log(canvas.height);
    canvas.height = 42;
    console.log(canvas.height);
    canvas.height = 1024;
    console.log(canvas.height);

    console.log(canvas.getContext());
    let gl = canvas.getContext();
    console.log(gl.COLOR_BUFFER_BIT);
}

export function canvasGLContextTest() {
    let canvas = new Canvas();
    const gl = canvas.getContext("webgl2");

    if (gl === null) {
        console.warn("WebGL 2 unsupport!");
    }

    gl.loadText2D("Holstein.DDS");

    const datas = [
        // [0.0, 0.0, 0.0, 1.0],
        [1.0, 0.0, 0.0, 1.0],
        [0.0, 1.0, 0.0, 1.0],
        [0.0, 0.0, 1.0, 1.0],
        // [1.0, 1.0, 0.0, 1.0],
        // [0.0, 1.0, 1.0, 1.0],
        // [1.0, 0.0, 1.0, 1.0],
    ];

    let y = 10;
    let height = canvas.height;
    // gl.clearColor(0.0, 0.0, 0.0, 1.0);

    let currentColor = datas[Math.floor(Math.random() * datas.length)];
    let fontSize = height / 10;
    let step = fontSize / 10;
    let frameTime = 10;

    function render() {
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.clearColor(...currentColor);
        // gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.drawText('V8 GL',
            20,
            y,
            80);
        y += step;
        if (y > height + fontSize) {
            y = 0;
            currentColor = datas[Math.floor(Math.random() * datas.length)];
        }

        if (!gl.tick()) {
            return;
        }
        setTimeout(render, frameTime);
    }

    render();
}
