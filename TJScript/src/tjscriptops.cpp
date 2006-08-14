#include "../include/internal/tjscript.h"
using namespace tj::script;
using namespace tj::shared;

/* OpPush */
OpPush::OpPush(ref<Scriptable> s) {
	_value = s;
}

std::wstring OpPush::GetName() {
	return std::wstring(L"OpPush ") + ScriptContext::GetValue<std::wstring>(_value, L"[unknown]");
}

void OpPush::Execute(ref<VM> vm) {
	vm->GetStack()->Push(_value);
}

void OpPop::Execute(ref<VM> vm) {
	vm->GetStack()->Pop();
}

void OpParameter::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> value = stack->Pop();
	ref<Scriptable> key = stack->Pop();
	ref<ScriptParameterList> parameter = stack->Pop();

	ref< ScriptValue<std::wstring> > keyString = key;
	parameter->Set(keyString->GetValue(), value);
	stack->Push(parameter);
}

void OpPushParameter::Execute(ref<VM> vm) {
	vm->GetStack()->Push(GC::Hold(new ScriptParameterList()));
}

void OpCall::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> parameterList = stack->Pop();
	ref<ScriptParameterList> list;
	if(parameterList.IsCastableTo<ScriptParameterList>()) {
		list = parameterList;
	}
	else {
		stack->Push(parameterList);
	}

	ref< ScriptValue<std::wstring> > funcName = stack->Pop();
	ref<Scriptable> target = stack->Pop();

	ref<Scriptable> result = target->Execute(funcName->GetValue(), list?list->_params:0);
	if(!result) {
		throw ScriptException(L"Variable does not exist on object or scope");
	}

	// only call functions when there is a parameter list given
	if(result.IsCastableTo<ScriptFunction>() && list) {
		ref<Scriptlet> scriptlet = ref<ScriptFunction>(result)->_scriptlet;
		vm->Call(scriptlet, list);
	}
	else {
		stack->Push(result);
	}
}

void OpIndex::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> index = stack->Pop();
	ref<Scriptable> object = stack->Pop();

	ref<ParameterList> pl = GC::Hold(new ParameterList());
	pl->insert(std::pair<std::wstring, ref<Scriptable> >(L"key", index));
	ref<Scriptable> result = object->Execute(L"get", pl);
	if(result==0) {
		throw ScriptException(L"Object does not support get(key=...) method, array index cannot be used");
	}
	stack->Push(result);
}


void OpCallGlobal::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> parameterList = stack->Pop();
	ref<ScriptParameterList> list;
	if(parameterList.IsCastableTo<ScriptParameterList>()) {
		list = parameterList;
	}
	else {
		stack->Push(parameterList);
	}

	ref< ScriptValue<std::wstring> > funcName = stack->Pop();
	ref<Scriptable> target = vm->GetGlobal();

	ref<Scriptable> result = target->Execute(funcName->GetValue(), list?list->_params:0);
	if(!result) {
		throw ScriptException(L"Variable does not exist on object or scope");
	}

	// only call functions when there is a parameter list given
	if(result.IsCastableTo<ScriptFunction>() && list) {
		ref<Scriptlet> scriptlet = ref<ScriptFunction>(result)->_scriptlet;
		vm->Call(scriptlet, list);
	}
	else {
		stack->Push(result);
	}
}

void OpSave::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();

	ref<Scriptable> object = stack->Pop();
	ref< ScriptValue<std::wstring> > varName = stack->Pop();
	vm->GetGlobal()->Set(varName->GetValue(), object);
}

void OpEquals::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> objectA = stack->Pop();
	ref<Scriptable> objectB = stack->Pop();

	bool result = false;

	if(objectA.IsCastableTo<ScriptString>() && objectB.IsCastableTo<ScriptString>()) {
		ref<ScriptString> strA = objectA;
		ref<ScriptString> strB = objectB;

		result = strA->GetValue() == strB->GetValue();
	}
	else if(objectA.IsCastableTo<ScriptInt>() && objectB.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> strA = objectA;
		ref<ScriptInt> strB = objectB;

		result = strA->GetValue() == strB->GetValue();
	}
	else if(objectA.IsCastableTo<ScriptDouble>() && objectB.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> strA = objectA;
		ref<ScriptDouble> strB = objectB;

		result = strA->GetValue() == strB->GetValue();
	}
	else if(objectA.IsCastableTo<ScriptBool>() && objectB.IsCastableTo<ScriptBool>()) {
		ref<ScriptBool> strA = objectA;
		ref<ScriptBool> strB = objectB;

		result = strA->GetValue() == strB->GetValue();
	}
	else if(objectA.IsCastableTo<ScriptNull>() && objectB.IsCastableTo<ScriptNull>()) {
		result = true;
	}
	else {
		result = (objectA==objectB);
	}

	stack->Push(GC::Hold(new ScriptBool(result)));
}

void OpNegate::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> top = stack->Pop();
	if(top.IsCastableTo<ScriptBool>()) {
		ref<ScriptBool> b = top;
		ref<ScriptBool> n = GC::Hold(new ScriptBool(!b->GetValue()));
		stack->Push(n);
	}
	else if(top.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> b = top;
		ref<ScriptInt> n = GC::Hold(new ScriptInt(-b->GetValue()));
		stack->Push(n);
	}
	else if(top.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> b = top;
		ref<ScriptDouble> n = GC::Hold(new ScriptDouble(-b->GetValue()));
		stack->Push(n);
	}
	else {
		stack->Push(GC::Hold(new ScriptBool(false)));
	}
}

void OpAdd::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> a = stack->Pop();
	ref<Scriptable> b = stack->Pop();

	if(a.IsCastableTo<ScriptInt>() && b.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> ia = a;
		ref<ScriptInt> ib = b;
		stack->Push(GC::Hold(new ScriptValue<int>(ia->GetValue()+ib->GetValue())));
	}
	else if(a.IsCastableTo<ScriptDouble>() && b.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> ia = a;
		ref<ScriptDouble> ib = b;
		stack->Push(GC::Hold(new ScriptValue<double>(ia->GetValue()+ib->GetValue())));
	}
	else if(a.IsCastableTo<ScriptString>()||b.IsCastableTo<ScriptString>()) {
		
		ref<ScriptString> ns = GC::Hold(new ScriptString(ScriptContext::GetValue<std::wstring>(b,L"")+ScriptContext::GetValue<std::wstring>(a,L"")));
		stack->Push(ns);
	}
	else {
		stack->Push(GC::Hold(new ScriptValue<int>(0)));
	}
}

void OpSub::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> a = stack->Pop();
	ref<Scriptable> b = stack->Pop();

	if(a.IsCastableTo<ScriptInt>() && b.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> ia = a;
		ref<ScriptInt> ib = b;
		stack->Push(GC::Hold(new ScriptInt(ib->GetValue()-ia->GetValue())));
	}
	else if(a.IsCastableTo<ScriptDouble>() && b.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> ia = a;
		ref<ScriptDouble> ib = b;
		stack->Push(GC::Hold(new ScriptDouble(ib->GetValue()-ia->GetValue())));
	}
	else {
		stack->Push(GC::Hold(new ScriptInt(0)));
	}
}

void OpDiv::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> a = stack->Pop();
	ref<Scriptable> b = stack->Pop();

	if(a.IsCastableTo<ScriptInt>() && b.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> ia = a;
		ref<ScriptInt> ib = b;
		stack->Push(GC::Hold(new ScriptDouble(double(ib->GetValue())/double(ia->GetValue()))));
	}
	else if(a.IsCastableTo<ScriptDouble>() && b.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> ia = a;
		ref<ScriptDouble> ib = b;
		stack->Push(GC::Hold(new ScriptDouble(ib->GetValue()/ia->GetValue())));
	}
	else {
		stack->Push(GC::Hold(new ScriptInt(0)));
	}
}

void OpMul::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> a = stack->Pop();
	ref<Scriptable> b = stack->Pop();

	if(a.IsCastableTo<ScriptInt>() && b.IsCastableTo<ScriptInt>()) {
		ref<ScriptInt> ia = a;
		ref<ScriptInt> ib = b;
		stack->Push(GC::Hold(new ScriptInt(ia->GetValue()*ib->GetValue())));
	}
	else if(a.IsCastableTo<ScriptDouble>() && b.IsCastableTo<ScriptDouble>()) {
		ref<ScriptDouble> ia = a;
		ref<ScriptDouble> ib = b;
		stack->Push(GC::Hold(new ScriptDouble(ia->GetValue()*ib->GetValue())));
	}
	else if(a.IsCastableTo<ScriptDouble>() && b.IsCastableTo<ScriptInt>()) {
		ref<ScriptDouble> da = a;
		ref<ScriptInt> ib = b;
		stack->Push(GC::Hold(new ScriptDouble(da->GetValue()*ib->GetValue())));
	}
	else if(b.IsCastableTo<ScriptDouble>() && a.IsCastableTo<ScriptInt>()) {
		ref<ScriptDouble> da = b;
		ref<ScriptInt> ib = a;
		stack->Push(GC::Hold(new ScriptDouble(da->GetValue()*ib->GetValue())));
	}
	else {
		stack->Push(GC::Hold(new ScriptInt(0)));
	}
}

OpBranchIf::OpBranchIf(int scriptlet) {
	_scriptlet = scriptlet;
}

void OpBranchIf::Execute(tj::shared::ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> top = stack->Top();
	
	bool r = ScriptContext::GetValue<bool>(top,false);
	if(r) {
		vm->Call(_scriptlet);
	}
}

// OpLoadScriptlet
OpLoadScriptlet::OpLoadScriptlet(int scriptlet) {
	_scriptlet = scriptlet;
}

void OpLoadScriptlet::Execute(tj::shared::ref<VM> vm) {
	ref<Scriptlet> sc = vm->GetScript()->GetScriptlet(_scriptlet);
	vm->GetStack()->Push(GC::Hold(new ScriptFunction(sc)));
}

void OpReturn::Execute(tj::shared::ref<VM> vm) {
	vm->Return(false);
}

void OpReturnValue::Execute(ref<VM> vm) {
	vm->Return(true);
}

// OpOr
void OpOr::Execute(ref<VM> vm) {
	ref<Scriptable> a = vm->GetStack()->Pop();
	ref<Scriptable> b = vm->GetStack()->Pop();

	bool ba = ScriptContext::GetValue<bool>(a, false);
	bool bb = ScriptContext::GetValue<bool>(b, false);

	vm->GetStack()->Push(GC::Hold(new ScriptBool(ba||bb)));
}

// OpAnd
void OpAnd::Execute(ref<VM> vm) {
	ref<Scriptable> a = vm->GetStack()->Pop();
	ref<Scriptable> b = vm->GetStack()->Pop();

	bool ba = ScriptContext::GetValue<bool>(a, false);
	bool bb = ScriptContext::GetValue<bool>(b, false);

	vm->GetStack()->Push(GC::Hold(new ScriptBool(ba&&bb)));
}

// OpXor: TT=>F FF=>F TF=>T FT=>T
// Xor: (a||b) && !(a==b)
void OpXor::Execute(ref<VM> vm) {
	ref<Scriptable> a = vm->GetStack()->Pop();
	ref<Scriptable> b = vm->GetStack()->Pop();

	bool ba = ScriptContext::GetValue<bool>(a, false);
	bool bb = ScriptContext::GetValue<bool>(b, false);

	vm->GetStack()->Push(GC::Hold(new ScriptBool( (ba||bb) && !(ba==bb) )));
}

void OpBreak::Execute(ref<VM> vm) {
	vm->Break();
}

void OpNew::Execute(ref<VM> vm) {
	ref<ScriptStack> stack = vm->GetStack();
	ref<Scriptable> parameterList = stack->Pop();
	ref<ScriptParameterList> list;
	if(parameterList.IsCastableTo<ScriptParameterList>()) {
		list = parameterList;
	}
	else {
		stack->Push(parameterList);
	}

	ref< ScriptString > funcName = stack->Pop();
	
	ref<Scriptable> instance = vm->GetContext()->GetType(funcName->GetValue())->Construct(list?list->_params:0);
	stack->Push(instance);
}

void OpIterate::Execute(ref<VM> vm) {
	ref<Scriptable> iterable = vm->GetStack()->Pop();
	ref<Scriptable> varName = vm->GetStack()->Pop();
	vm->GetStack()->Push(varName);
	vm->GetStack()->Push(iterable);
	
	ref<Scriptable> value = iterable->Execute(L"next", 0);
	if(value==0) {
		throw ScriptException(L"Object does not support iteration");
	}
	else if(!value.IsCastableTo<ScriptNull>()) {
		// set pc of this stack frame one back, so this instruction will be called again
		ref<StackFrame> frame = vm->GetStackFrame();
		frame->_pc--;

		//set variable
		vm->GetGlobal()->Set(ref<ScriptString>(varName)->GetValue(), value);

		vm->Call(_scriptlet);
	}
	else {
		vm->GetStack()->Pop();
		vm->GetStack()->Pop();
	}
}

