function hannuota(n,a,b,c) {
  if(n == 0) {
	return 0;
  }
  if(n == 1) {
	debug(a + "->" + c);
	return 1;
  }
  return hannuota(n - 1, a, c, b) + hannuota(1, a, b, c) + hannuota(n - 1, b, a, c);
}

let n = 13;
let timeStart = milliseconds();
debug("HNT" + n + ":" + hannuota(n,"a","b","c"));
let timeEnd = milliseconds();
debug("Elapse milliSeconds:" + (timeEnd - timeStart));
debug("finish");