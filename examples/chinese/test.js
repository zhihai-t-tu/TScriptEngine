function 转换(s) {
	return s.replace("!!","$$");
}

我爱你中国="I Love You, China";
我爱你中国.append("!!");
debug(转换(我爱你中国));
