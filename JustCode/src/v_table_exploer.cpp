#include <iostream>
#include <stdint.h>

class Base
{
public:
	virtual void f() { std::cout << "Base::f()" << std::endl; }
	virtual void g() { std::cout << "Base::g()" << std::endl; }
	virtual void h() { std::cout << "Base::h()" << std::endl; }
};

class Derive : public Base
{
public:
	virtual void f() { std::cout << "Derive::f()" << std::endl; }
	virtual void g1() { std::cout << "Derive::g1()" << std::endl; }
	virtual void h1() { std::cout << "Derive::h1()" << std::endl; }
};

class Animal
{
protected:
	int age_;

public:
	Animal(int age = 0) : age_(age) {}
	virtual void PrintAge() { std::cout << "Animal! I'm " << age_ << " old." << std::endl; }
};

class Dog : Animal
{
public:
	Dog(int age = 0) : Animal(age) {}
	virtual void PrintAge() { std::cout << "Dog! I'm " << age_ << " old." << std::endl; }
};

class Cat : Animal
{
public:
	Cat(int age = 0) : Animal(age) {}
	virtual void PrintAge() { std::cout << "Cat! I'm " << age_ << " old." << std::endl; }
};

typedef void (*Fun)(void);

int main()
{
	// print length
	std::cout << "The Length: " << std::endl;
	std::cout << "length of pointer: " << sizeof(void*) << std::endl;
	std::cout << "length of long: " << sizeof(long) << std::endl;
	std::cout << "length of int: " << sizeof(int) << std::endl;
	std::cout << "length of unsigned int: " << sizeof(unsigned int) << std::endl;
	std::cout << "length of intptr_t: " << sizeof(intptr_t) << std::endl;
	std::cout << "length of uintptr_t: " << sizeof(uintptr_t) << std::endl;
	std::cout << "=====================\n\n";

	// print virtual function table;
	Derive d;
	Base* ptr = &d;
	Fun ptr_func = NULL;

	uintptr_t address_v_table = (uintptr_t)*(uintptr_t*)(ptr);
	uintptr_t address_func_0 = *((uintptr_t*)address_v_table);

	std::cout << "v-table address: " << address_v_table << std::endl;
	std::cout << "first function address in v-table: " 
				<< address_func_0 << std::endl;

	for (int i=0; i<5; ++i)
	{
		ptr_func = (Fun)*((uintptr_t*)address_v_table + i);
		(*ptr_func)();
	}
	std::cout << "=====================\n\n";

	// change v-table, but note *this is not change!
	Dog dog(5);
	Cat cat(6);

	std::cout << "Dog and cat: " << std::endl;
	dog.PrintAge();
	cat.PrintAge();
	std::cout << "\nexchange v-table" << std::endl;

	uintptr_t *ptr_v_table_dog = (uintptr_t*)&dog;
	uintptr_t *ptr_v_table_cat = (uintptr_t*)&cat;
	uintptr_t tmp;

	tmp = *ptr_v_table_dog;
	*ptr_v_table_dog = *ptr_v_table_cat;
	*ptr_v_table_cat = tmp;

	Animal *ptr_animal = (Animal*)&dog;
	ptr_animal->PrintAge();
	ptr_animal = (Animal*)&cat;
	ptr_animal->PrintAge();

	return 0;
}