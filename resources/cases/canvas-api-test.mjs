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