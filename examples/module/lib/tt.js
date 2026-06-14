class ClassA
{
   let className;
	let valueName = "InitA";
	function ClassA(param) {
      className = "ClassA";
  		 valueName = "[" + className + "-" + param + "|A|" + valueName + "]";
	}
	function ~ClassA() {
      debug("ClassA Done");
	}
   function displayClassA() {
       debug("ClassA:" + valueName);
   }
	function displayClass1() {
		displayClassA();
	}
	function displayClass() {
		displayClass1();
	}
};
