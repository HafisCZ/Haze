#pragma once

#include "Scanner.h"
#include "AST.h"

namespace INL 
{

	void ReaderTest() {
		std::string source = "for (var o : scene.objects(5)) { o.rotation.x += '75.5'; { //retarded cunt\n }} while(true) { doNothing(); }";

		Scanner scanner(source);
		AST ast(scanner.Get());
		
		for (auto expr : ast.Get()) {
			expr->Evaluate();
		}
	}

}