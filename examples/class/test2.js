class Spline {
    let x = [];
    let y = [];
    let a = [];
    let b = [];
    let c = [];
    let d = [];
	function Spline(px, py, pa, pb, pc, pd) {
		x = px;
		y = py;
		a = pa;
		b = pb;
		c = pc;
		d = pd;
	}
};
// 计算三次样条插值
function cubicSplineInterpolation(x, y) {
    let n = x.size();
    let h = [];h.resize(n - 1);
    let a = [];a.resize(n);
    let b = [];b.resize(n);
    let c = [];c.resize(n);
    let d = [];d.resize(n);
    let l = [];l.resize(n);
    let mu = [];mu.resize(n);
    let z = [];z.resize(n);

    // 初始化
    for (let i = 0; i <( n - 1); i ++) {
        h[i] = x[i + 1] - x[i];
    }
    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;
    // 构造矩阵方程
    for (let i = 1; i < (n - 1); i ++) {
        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = 1.0 * h[i] / l[i];
        z[i] = 1.0 * (y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1];
        z[i] = 1.0 * (z[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    l[n - 1] = 1;
    z[n - 1] = 0;
    c[n - 1] = 0;
    // 回代求解c
    for (let i = n - 2; i >= 0; i--) {
        c[i] = z[i] - mu[i] * c[i + 1];
        b[i] = 1.0 * (y[i + 1] - y[i]) / h[i] - h[i] * (c[i + 1] + 2 * c[i]) / 3;
        d[i] = 1.0 * (c[i + 1] - c[i]) / (3 * h[i]);
        a[i] = y[i];
    }
    return Spline(x, y, a, b, c, d);
}
// 使用样条进行插值
function evaluateSpline(spline, x_val) {
    // 找到x_val所在的区间
    let i = 0;
    while (i < (spline.x.size() - 1) && x_val > (spline.x[i + 1])) {
        i ++;
    }
    // 计算插值结果
    let dx = x_val - spline.x[i];
    return spline.a[i] + spline.b[i] * dx + spline.c[i] * dx * dx + spline.d[i] * dx * dx * dx;
}

// 打印样条系数
function printSpline(spline) {
    debug("Spline coefficients:\n");
	
    for (let i = 0; i < (spline.x.size() - 1); i++) {
        debug( "Interval [" + spline.x[i] + ", " + spline.x[i + 1] + "]:\n");
        debug( "a: " + spline.a[i] + ", b: " + spline.b[i] + ", c: " + spline.c[i] + ", d: " + spline.d[i] + "\n");
    }
}
function main() {
    let x = [ 0, 1, 2, 3, 4 ,5, 6, 7]; // 数据点的x坐标
    let y = [ 1, 2, 0, 2, 1 ,0, -1, -2]; // 数据点的y坐标
    // 计算三次样条插值
    let spline = cubicSplineInterpolation(x, y);
    // 打印样条系数
    printSpline(spline);
    // 在某个点进行插值
    let x_val = 6.5;
    let result = evaluateSpline(spline, x_val);
    debug( "Interpolated value at x = " + x_val + " is " + result );
    return 0;
}
main();
let position = [ [ 0, 0 ], [ 5000, 100000 ], [ 50000, 150000 ], [ 100000, 120000 ], [ 250000, -5500 ] ];
let xx = [];
let yy = [];
for(let i=0;i<position.length();i++) {
  xx << position[i][0];
  yy << position[i][1];
}
let spline = cubicSplineInterpolation(xx, yy);
debug("yy=" + evaluateSpline(spline, 111000));