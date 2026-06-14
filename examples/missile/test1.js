function debug(s) {
	output(s);
}

T=86164;	//地球自传一天的周期
PI=3.1415926535897;
a=6378137.0;
b=6356752.3;
ee=(1-b*b/(a*a));
e=sqrt(ee);		//第一偏心率

function rad2degree(rad) {
	return rad * 180 / PI;
}

function degree2rad(degree) {
	return degree * PI / 180;
}

//经纬度坐标转地心坐标
function lbhdegree2xyz(lon,lat,h) {
	let lonRad = degree2rad(lon);
	let latRad = degree2rad(lat);
	
	let sinlon = 0;
	let coslon = 0;
	let sinlat = 0;
	let coslat = 0;
	if(lon == 0) {
		coslon = 1;
	} else if(lon == 90) {
		sinlon = 1;
	} else if(lon == 180) {
		coslon = -1;
	} else if(lon == -90) {
		sinlon = -1;
	} else {
		sinlon = sin(lonRad);
		coslon = cos(lonRad);
	}
	if(lat == -90) {
		return [0,0,-(h+b)];
	} else if(lat == 0) {
		coslat = 1;
	} else if(lat == 90) {
		return [0,0,h+b];
	} else {
		sinlat = sin(latRad);
		coslat = cos(latRad);
	}
	
	let N=a/sqrt(1-ee*sinlat*sinlat);

	let x=(N+h)*coslon*coslat;
	let y=(N+h)*sinlon*coslat;
	let z=((1-ee)*N+h)*sinlat;

	return [x,y,z];
}

//经纬弧度坐标转地心坐标
function lbh2xyz(L,B,H) {
	let sinlon = sin(L);
	let coslon = cos(L);
	let sinlat = sin(B);
	let coslat = cos(B);
	
	let N=a/sqrt(1-ee*sinlat*sinlat);

	let x=(N+H)*coslon*coslat;
	let y=(N+H)*sinlon*coslat;
	let z=((1-ee)*N+H)*sinlat;

	return [x,y,z];
}

//地心坐标转经纬弧度--近似算法
function xyz2lbh(x,y,z) {
	let p = sqrt(x*x+y*y);
	let L = atan2(y,x);
	if(p == 0) {
		if(z >= 0) {
			let B=90;
			let H=z-b;
			return [L,B,H];
		} else {
			let B=-90;
			let H=(-z)-b;
			return [L,B,H];
		}
	}
	if(z == 0) {
		let B=0;
		let H=p-a;
		return [L,B,H];
	}
	
	let theta = atan(z * a/(p*b));
	let B = atan((z + ee/(1-ee)*b*sin(theta)*sin(theta)*sin(theta))/(p - ee*a*cos(theta)*cos(theta)*cos(theta)));
	let N = a/sqrt(1-ee*sin(B)*sin(B));
	let H = p / cos(B) - N;
	return [L,B,H];
}

//地心坐标转经纬弧度--迭代算法
function xyz2lbh_n(x,y,z) {
	function ite(z,p) {
		function cal_N(B) {
			return a/sqrt(1-ee*sin(B)*sin(B));
		}
		function cal_H(p, N, B) {
			return p/cos(B)-N;
		}
		function cal_B(z,p,N,H) {
			return atan(z/((1 - ee*N/(N+H))*p));
		}

		let B=cal_B(z,p,1,0);
		let N=cal_N(B);
		let H0=1000000000.0;
		let H=cal_H(p,N,B);
		let loop = 0;
		while (abs(H - H0) > 0.00001) {
			B = cal_B(z,p,N,H);
			N = cal_N(B);
			H0=H;
			H=cal_H(p, N, B);
			loop ++;
			if(loop > 2000) {
				break;
			}
			//debug("loop=" + loop + ";H=" + H + ";H0=" + H0 + ";B=" + rad2degree(B));
		}
		return [H,B];
	}

	let p = sqrt(x*x+y*y);
	let L = atan2(y,x);
	if(p == 0) {
		if(z >= 0) {
			let B=90;
			let H=z-b;
			return [L,B,H];
		} else {
			let B=-90;
			let H=(-z)-b;
			return [L,B,H];
		}
	}
	if(z == 0) {
		let B=0;
		let H=p-a;
		return [L,B,H];
	}
	
	let VA = ite(z,p);
	let H = VA[0];
	let B = VA[1];
	return [L,B,H];
}

function xyz2espeed(x,y) {
	return sqrt(x*x+y*y)*2*PI/T;
}

Gn=6.674;		//6.67259E-11  N·m²/kg²		万有引力常量
Mn=5.97237;		//5.965E24  kg				地球质量

//万有引力加速度
function xyz2G(x,y,z) {
	//地心距离
	let R=sqrt(x*x + y*y + z*z);

	//万有引力加速度
	return (Gn*Mn*10)/((R/1000000)*(R/1000000));
}

//xyz重力加速度
function xyz2g(x,y,z) {
	//旋转半径
	let p=sqrt(x*x+y*y);
	//地心距离
	let R=sqrt(x*x + y*y + z*z);

	//地球同步转动速度
	let speed = p*2*PI/T;

	//向心加速度
	let glat=p*2*PI/T*2*PI/T;

	//万有引力加速度
	let gg=(Gn*Mn*10)/((R/1000000)*(R/1000000));
	let g= sqrt(glat*glat+gg*gg-2*glat*gg*p/R);
	debug("R=" + R + ";speed=" + speed + ";gg=" + gg + ";glat=" + glat + ";g=" + g);
	return g;
}

//g:加速度；s:初始速度；ts:移动时间
//返回：[距离，最终速度]
function movelength(g,v,ts) {
	let fv = v + g*ts;
	let dl = (v + fv) * ts/2;
	return [dl,fv];
}

//计算三个方向的分量比例
function getxyzfactor(x,y,z) {
	let R=sqrt(x*x+y*y+z*z);
	return [x/R,y/R,z/R];
}


//计算三个方向的分量值
function getv2xyzfactor(v,x,y,z) {
	let R=sqrt(x*x+y*y+z*z);
	return [v*x/R,v*y/R,v*z/R];
}

function getraddistance(fromL, fromB, toL, toB, H) {
	let fromXYZ = lbh2xyz(fromL, fromB, H);
	let toXYZ = lbh2xyz(toL, toB, H);
	let BB = fromXYZ[0] * fromXYZ[0] + fromXYZ[1] * fromXYZ[1] + fromXYZ[2] * fromXYZ[2];
	let CC = toXYZ[0] * toXYZ[0] + toXYZ[1] * toXYZ[1] + toXYZ[2] * toXYZ[2];
	let AA = (fromXYZ[0] - toXYZ[0]) * (fromXYZ[0] - toXYZ[0]) + (fromXYZ[1] - toXYZ[1]) * (fromXYZ[1] - toXYZ[1]) + (fromXYZ[2] - toXYZ[2]) * (fromXYZ[2] - toXYZ[2]);

	return sqrt(BB) * acos((BB + CC - AA)/(2*sqrt(BB*CC)));
}
function getrad(fromL, fromB, toL, toB) {
	let fromXYZ = lbh2xyz(fromL, fromB, 0);
	let toXYZ = lbh2xyz(toL, toB, 0);
	let BB = fromXYZ[0] * fromXYZ[0] + fromXYZ[1] * fromXYZ[1] + fromXYZ[2] * fromXYZ[2];
	let CC = toXYZ[0] * toXYZ[0] + toXYZ[1] * toXYZ[1] + toXYZ[2] * toXYZ[2];
	let AA = (fromXYZ[0] - toXYZ[0]) * (fromXYZ[0] - toXYZ[0]) + (fromXYZ[1] - toXYZ[1]) * (fromXYZ[1] - toXYZ[1]) + (fromXYZ[2] - toXYZ[2]) * (fromXYZ[2] - toXYZ[2]);

	return acos((BB + CC - AA)/(2*sqrt(BB*CC)));
}
function getarc(a, b, c) {
	return acos(1.0*(b*b+c*c-a*a)/(2*b*c));
}
function getr(x, y ,z) {
	return sqrt(x*x+y*y+z*z);
}


function getdirect(L, B, orientation, elevation) {
	let xyz = [sin(degree2rad(elevation)),cos(degree2rad(elevation)),0];
	xyz = [xyz[0], xyz[1] * cos(degree2rad(orientation)), xyz[1] * sin(degree2rad(orientation))];
	
	let R = getr(xyz[0],0,xyz[2]);
	let arc = atan2(xyz[2],xyz[0]) + degree2rad(B);
	xyz = [R*cos(arc),xyz[1],R*sin(arc)];
	R = getr(xyz[0],xyz[1],0);
	arc = atan2(xyz[1],xyz[0])+degree2rad(L);
	return [R*cos(arc),R*sin(arc),xyz[2]];
}

let airDensityTable = [[-500, 1.396],[0, 1.225],[500, 1.167],[1000, 1.112],[1500, 1.058],[2000, 1.006],[2500, 0.957],[3000, 0.909],[3500, 0.863],[4000, 0.819],[4500, 0.777],[5000, 0.736],[5500, 0.697],[6000, 0.660],[6500, 0.624],[7000, 0.590],[7500, 0.558],[8000, 0.527],[9000, 0.467],[10000, 0.413],[11000, 0.364],[12000, 0.311],[1000000, 0]];
function getAirDensity2(H, pos) {
	let tlen = airDensityTable.length();
	if(pos < 0) pos = 0;
	for(let i=(pos + 1);i<tlen;i++) {
		if(airDensityTable[i - 1][0] <= H && H < airDensityTable[i][0]) {
			return [i,((H - airDensityTable[i - 1][0])/(airDensityTable[i][0]-airDensityTable[i - 1][0]))*(airDensityTable[i][1] - airDensityTable[i - 1][1])+airDensityTable[i - 1][1]];
		}
	}
	if(H < -500) return [0,airDensityTable[0][1]];
	return [tlen - 1,0];
}


function getAirDensity(H) {
	return getAirDensity2(H,0)[1];
}

// 空气动力学参数
rHo = 1.225; // 空气密度（kg/m³，海平面标准值，高度越高，空气重力越小，空气上下层压力越小，空气密度越低，影响因素：高度，重力加速度=>空气密度），先假设不变
rCd = 0.1; // 气动阻力系数（无量纲）
objA = 0.05*0.05*PI/2; // 参考迎风面积（m²）
mm = 2000;//kg

function getAirForce(speed) {
	return 0.5 * rHo * rCd * objA * speed*speed;
}
function getAirg(speed) {
	return 0.5 * rHo * rCd * objA * speed*speed / mm;
}

function missile_dynamic(x, y, z, vx, vy, vz, t, ts) {
	let G = xyz2G(x,y,z);
	//计算空气阻力：计算速度，除去地球自转速度
	let ovy = vy - xyz2espeed(x,y);
	let airg = [getAirg(vx),getAirg(ovy),getAirg(vz)];
	let factor = getxyzfactor(x,y,z);
	let dxv = movelength(-factor[0]-airg[0],vx,ts);
	let dyv = movelength(-factor[1]-airg[1],vy,ts);
	let dzv = movelength(-factor[2]-airg[2],vz,ts);
	return [x + dxv[0], y + dyv[0], z + dzv[0], dxv[1], dyv[1], dzv[1], t + ts, 0];
}
function outputLBH(n,l,b,h) {
	debug("n=" + n + ";l=" + l + ";b=" + b + ";h=" + h);
}
function main() {
	let fromLBH = [119,21,0];
	let direct = [30,30];//正东,仰角
	let speed = 200;
	let toLBH = [121,23,0];
	let xyz = lbhdegree2xyz(0,fromLBH[1],fromLBH[2]);
	let VFactor = getdirect(0,fromLBH[1],direct[0],direct[1]);
	let dyv = xyz2espeed(xyz[0],xyz[1]);
	let ts = 0.1;
	let currparam = [xyz[0],xyz[1],xyz[2],VFactor[0] * speed, VFactor[1] * speed + dyv, VFactor[2] * speed, 0, 0];
	let t = 0;
	let loop = 0;
	outputLBH(1,fromLBH[0],fromLBH[1],fromLBH[2]);
	let prevLBH = fromLBH;
	let prevRHO = getAirDensity2(fromLBH[2],0);
	rHo = prevRHO[1];
	for(;;) {
		loop ++;
		currparam = missile_dynamic(currparam[0],currparam[1],currparam[2],currparam[3],currparam[4],currparam[5], t, ts);
		let LBH = xyz2lbh(currparam[0],currparam[1],currparam[2]);
		LBH = [LBH[0]*180/PI + fromLBH[0] - 360 * t / T,LBH[1]*180/PI,LBH[2]];//由于地球自转，坐标系跟着自西向东旋转，坐标系旋转后，东向的值应变少，而实际计算的时候，假定坐标系未旋转，因而值相对旋转后的值，变多了，所以需要减去一个旋转经度差
		if(LBH[2] < 0) {
			let fs = prevLBH[2]/(prevLBH[2] - LBH[2]);
			let fL = prevLBH[0] - fs * (prevLBH[0] - LBH[0]);
			let fB = prevLBH[1] - fs * (prevLBH[1] - LBH[1]);
			outputLBH(1,fL,fB,0);
			break;
		}
		outputLBH(1,LBH[0],LBH[1],LBH[2]);
		prevLBH = LBH;
		prevRHO = getAirDensity2(LBH[2],prevRHO[0] - 1);
		rHo = prevRHO[1];
		//debug("LOOP:" + loop + ";L:" + LBH[0] + ";" + "B:" + LBH[1] + ";H:" + LBH[2]);
		t += ts;
	}
}
main();
