
T=86164;    //地球自传一天的周期
PI=3.1415926535897;
a=6378380.0;
b=6356908.8;
ee=(1-b*b/(a*a));
e=sqrt(ee);        //第一偏心率

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
    let lonRad = degree2rad(L);
    let latRad = degree2rad(B);

    let sinlon = sin(lonRad);
    let coslon = cos(lonRad);
    let sinlat = sin(latRad);
    let coslat = cos(latRad);
    
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
            debug("loop=" + loop + ";H=" + H + ";H0=" + H0 + ";B=" + rad2degree(B));
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

function xyz2speed(x,y) {
    return sqrt(x*x+y*y)*2*PI/T;;
}

Gn=6.674;        //6.67259E-11  N·m²/kg²        万有引力常量
Mn=5.97237;        //5.965E24  kg                地球质量

//万有引力加速度
function xyz2gg(x,y,z) {
    //地心距离
    let R=sqrt(x*x + y*y + z*z);

    //万有引力加速度
    let gg=(Gn*Mn*10)/((R/1000000)*(R/1000000));
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


EA=lbhdegree2xyz(121.022333, 34.567768,10);

let x= EA[0];
let y= EA[1];
let z= EA[2];

let RA2 = xyz2lbh_n(x,y,z);
debug("RAn:L=" + rad2degree(RA2[0]) + ";B=" + rad2degree(RA2[1]) + ";H=" + RA2[2]);

let RA = xyz2lbh(x,y,z);

debug("RA0:L=" + rad2degree(RA[0]) + ";B=" + rad2degree(RA[1]) + ";H=" + RA[2]);

debug("XYZ:X=" + x + ";Y=" + y + ";Z=" + z);

debug("g=" + xyz2g(x,y,z));

//旋转半径
let p=sqrt(x*x+y*y);
//地心距离
let R=sqrt(x*x + y*y + z*z);

//地球同步转动速度
let speed = p*2*PI/T;

//向心加速度
glat=p*2*PI/T*2*PI/T;

//万有引力加速度
let gg=(Gn*Mn*10)/((R/1000000)*(R/1000000));


//重力加速度
g=sqrt(glat*glat+gg*gg-2*glat*gg*p/R);

debug("R=" + R + ";speed=" + speed + ";gg=" + gg + ";glat=" + glat + ";g=" + g);
