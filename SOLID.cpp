#include <iostream>
#include<vector>
#include<tuple>
#include<fstream>
#include<string>
#include<unordered_map>
using namespace std;
/**
 S - Single responsibelity function - One class should have only one reason to change and handle seperate concerns
 O- Open/Close principle - Open to extension and close to modification
 L- Liskov substitution principle- Subtypes should be substitutable by base types
 I- Interface segregation principle- have leaner interfaces
 D- Dependecy intersion - high level modules should not depend on low level modules , both should depend on abstraction
 */
/*
 SINGLE RESPONSIBELITY PRINCIPLE - sepration of concerns
    no class should have a secondary purpose-ideally
 **/
struct Journal{
    string title;
    vector<string> entries;
    explicit Journal(const string& title):title(title){}
    // Note: explict is generally used with single arg constructors
    void add (const string& entry);
    void save (const string & filename);// // persistence is a separate concern
};
void Journal::add(const string& entry){
    static int count=1;
    entries.push_back(to_string(count++)+": "+entry);
}
void Journal::save(const string & filename){// wrong way to do stuff==
    ofstream ofs(filename);
    for(auto& stringEntries:entries){
        ofs<<stringEntries<<endl;
    }
}
struct PersistenceManager{// when you want to change persistence -change this -not everywheere
    static void save (const Journal& j, const string& filename){
        ofstream ofs(filename);
        for(auto& stringEntries:j.entries)
            ofs<<stringEntries<<endl;
    }
};
//int  main(){
//    Journal journal("Dear Diary");
//    journal.add("I worked hard today");
//    journal.add("I worked hard again today");
//   // journal.save("diary.txt"); wrong
//    PersistenceManager pm;
//    pm.save(journal, "diary.txt");
//    return 0;
//}

//---------------------------------------------------------------------------------------------
/*
    OPEN CLOSED PRINCIPLE
    - Your code should be open to extension(by inheritance) but closed to modification
 **/

enum class Color {red,green,blue};
enum class Size {small,medium,large};
//enumerator names are local to the enum and their values do not implicitly convert to other types (like another enum or int)
struct Product {
    string name; Color color; Size size;
};
struct ProductFilter {// for 2 items 3 such fxns are required, for n items 2^n-1 functions are required- ** bad extendabelity **
    vector<Product*> by_color(vector<Product*> items, Color color){
        vector<Product*> result;
        for(auto&i :items){
            if(i->color==color) result.push_back(i);
        }
        return result;
    }
    vector<Product*> by_size(vector<Product*> items, Size size){
        vector<Product*> result;
        for(auto&i :items){
            if(i->size==size) result.push_back(i);
        }
        return result;
    }
    
    vector<Product*> by_size_and_color(vector<Product*> items, Size size,Color color){
        vector<Product*> result;
        for(auto&i :items){
            if(i->size==size && i->color==color) result.push_back(i);
        }
        return result;
    }
};
//----------------------------------------- SPECIFICATION PATTERN (ENTERPRISE PATTERN) - excellent **extendabelity**
template<typename T> struct Specification{
    virtual bool is_satisfied(T* item)=0;
};
template<typename T> struct Filter{
    virtual vector<T*>  filter(vector<T*> items, Specification<T>& spec)=0;// filters given the specificaiton
};// can be used not just by Product by also any other object type with its Specifiaction

struct BetterFilter:Filter<Product>{
    vector<Product*> filter(vector<Product *> items, Specification<Product>& spec) override{
        vector<Product*> result;
        for(auto&i :items){
            if(spec.is_satisfied(i)) result.push_back(i);
        }
        return result;
    }
};
// creating a combinatior to combine specifications
template<typename T> struct AndSpecifiaction: Specification<T>{
    Specification<T>& first;
    Specification<T>& second;
    AndSpecifiaction(Specification<T>& first,
                     Specification<T>& second):first(first),second(second){}
    bool is_satisfied (T* item) override{
        return first.is_satisfied(item) && second.is_satisfied(item);
    }
};
struct ColorSpecificaiton: Specification<Product>{
    Color color;
    ColorSpecificaiton(Color color):color(color){};
    bool is_satisfied(Product* item) override {
        return item->color==color;
    }
};
struct SizeSpecificaiton: Specification<Product>{
    Size size;
    explicit SizeSpecificaiton(const Size size):size(size){};
    bool is_satisfied(Product* item) override {
        return item->size==size;
    }
};


//int main(){
//    Product apple{"Apple",Color::red,Size::small};// can only init structs this way
//    Product tree{"Tree",Color::green,Size::large};
//    Product bush{"Bush",Color::green,Size::large};
//    Product grass{"Grass",Color::green,Size::small};
//    Product sky{"Sky",Color::blue,Size::large};
//    vector<Product*> items{&apple, &tree, &sky,&bush,&grass,&sky};
//    ProductFilter pf;
//    vector<Product*> green_things=pf.by_color(items, Color::green);
//    // Better Filter has better extendabelity
//    BetterFilter bf;
//    ColorSpecificaiton green(Color::green);
////    for(auto& item:bf.filter(items,green)){
////        cout<<item->name<<endl;
////    }
//    SizeSpecificaiton large(Size::large);
//    AndSpecifiaction<Product> green_and_large(green,large);
//    for(auto& item:bf.filter(items,green_and_large)){
//        cout<<item->name<<endl;
//    }
//
//    return 0;
//}
//---------------------------------------------------------------------------------------------
/* LISKOV SUBSITUTION PRINCIPLE- Subtypes should be immidieately subsitutate for base types
    -> If you have a function which takes a base class any derived class should be able to be subsituted into this function */
class Rectangle {
protected:
    int width,height;
public:
    Rectangle(int width,int height): width(width),height(height){}// Note:Initializer list
    int getWidth() const {// Note :const after a function declaration, makes it a compiler error for this class function to change a member variable of the class
        return width;
    }
    virtual void setWidth(int width){
        Rectangle::width=width;// Note : Scope rresolution to prevent confusion from global/local variables of the same name .Can also use this ->
    }
    int getHeight()const {
        return height;
    }
    virtual void setHeight(int height){// Note: Virtual simply means that this function is late-binded
        Rectangle::height=height;
    }
    int area() const{
        return height*width;
    }
};
void process(Rectangle& r){// THIS FUNCTION LOGIC IS TRUE CONSIDERING BASE TYPE BUT FAlSE CONSIDERTING SUBTYPES ie Square
    int w= r.getWidth();
    r.setHeight(10);
    cout<<"Expected area: "<<(w*10)<<", got "<<r.area()<<endl;
}
class Sqaure:public Rectangle{// NOTE: if you do not explicitly define access specifier Defaults to private inheritance
public:
    Sqaure(int size):Rectangle(size,size){}//NOTE: child class calling parents constructor using initializer list
    void setHeight(int height) override{// NOTE:Using override The compiler also knows that it's an override, so it can "check" that you are not altering/adding new methods that you think are overrides, as base case defiding a function with same name but diffeenrt agruments / of differnt types would amount to function hiding and not overriding
        this->width=this->height=height;
    }
    void setWidth(int width) override{
        this->width=this->height=width;
    }
    
};

//int main(){
//    cout<<"Design Principles "<<endl;
//    Rectangle r{3,4};
//    Sqaure sq{5};
//    process(r);
//    process(sq);// VIOLATES LISKOV SUBSTITUTION PRINCIPLE- making sqaure class violated the principle as process got invalid
//
//    return 0;
//}
// One way to maintian Liskov's substition principlw would be to create a Rectangle factory
struct RectangleFactory{
    static Rectangle create_rectangle(int w,int h);
    static Rectangle create_square(int size);
};


//---------------------------------------------------------------------------------------------
/**
    INTERFACE SEGREGATION PRICIPLE - do not create interfaces which are too large- avoid stuffing too muchj
            If we have only an interface which has printer/scanner/fax what happends when we only want a fax machine ?
 */
struct Document{};
struct IMachine{// INTERACE FOR SCANNING AND PRINTING AND FAXING- violates Interface segregation principle
    virtual void print(Document& doc)=0;
    virtual void scan(Document& doc)=0;
    virtual void fax(Document& doc)=0;
};
struct MFP: IMachine{// MultifunctionPrinter struct implements the interface-default(public) visibility mode
    void print (Document &doc) override {}
    void scan (Document &doc) override {}
    void fax (Document &doc) override {}
};
struct Scanner: IMachine{
    void print (Document &doc) override {}// HAS NO MEANING HERE
    void scan (Document &doc) override {}
    void fax (Document &doc) override {}// HAS NO MEANING HERE
};
// SO IDEA IS TO SEGREGATE THE INTERFACES
struct Iprinter{
    virtual void print(Document& doc)=0;
};
struct Iscan{
    virtual void scan(Document& doc)=0;
};
struct Ifax{
    virtual void fax(Document& doc)=0;
};
struct ScannerAndPrinter:Iprinter, Iscan{
    void print (Document &doc) override {
        cout<<"you print"<<endl;
    }
    void scan (Document &doc) override {
        cout<<"you scan"<<endl;
    }
};
// DECORATOR PATTERN
struct Machine: Iprinter,Iscan {
    Iprinter & printer;
    Iscan& scanner;
    Machine(Iprinter &printer,Iscan& scanner):printer(printer),scanner(scanner){}
    void print (Document &doc) override {
        printer.print(doc);
    }
    void scan (Document &doc) override {
        scanner.scan(doc);
    }
};
//int main(){
//    Document d;
//    ScannerAndPrinter sp;
//    sp.print(d);
//    return 0;
//}
//---------------------------------------------------------------------------------------------
/**
    DEPENDENCY INVERSION PRINCIPLE-  specifies best way to form dependecies between objects
            1  High level modules should not depend on low -level modules- both should depend on abstractions
                        2 Abstractions should not depend on details . Details should depend upon abstractions
 -> have dependecies on interfaces - rather than low level modules
 */
enum class Relationship{parent,child,sibling};// NOTE- this is an enum class and not an enum type !
struct Person{
    string name;
};
//struct Relationships{// LOW LEVEL MODULE/CONSTRUCT- to add relationships between people
//    vector<tuple<Person,Relationship,Person>> relations;
//    void add_parent_and_child(const Person& parent,const Person& child){
//        relations.push_back({parent,Relationship::parent,child});
//        relations.push_back({child,Relationship::child,parent});
//    }
//};

struct RelationshipBrowser{
    virtual vector<Person> find_all_children_of(const string & name)=0;
};
struct Relationships:RelationshipBrowser{// LOW LEVEL MODULE/CONSTRUCT
    vector<tuple<Person,Relationship,Person>> relations;
    void add_parent_and_child(const Person& parent,const Person& child){
        relations.push_back({parent,Relationship::parent,child});
        relations.push_back({child,Relationship::child,parent});
    }
    vector<Person> find_all_children_of(const string & name) override{// research functionality moved to low level module
        vector<Person> result;
        for(auto&& [first,rel,second]:relations){
            if(first.name==name && rel==Relationship::parent){
                result.push_back(second);
            }
        }
        return  result;
    };
};

struct BetterResearch{// has dependecy on an interface not low level module
    BetterResearch(RelationshipBrowser& browser){
        for(auto& child:browser.find_all_children_of("Jhon")){
            cout<<"Jhon has a child called "<<child.name<<endl;
        }
    }
};

struct Research {// HIGH LEVEL MODULE- Looking for children of jhon
    Research(Relationships& relationships){// THIS LINE VIOLATES DEPENDENCY INSVERSION , as Relationships in a low level module
        //for now we know relationships is vector what if it changes to a Map ? set ? or changes memeber variable?
        auto& relations=relationships.relations;
        for(auto&& [first,rel,second]:relations){// TRAVERSING A TUPLE
            if(first.name=="Jhon" && rel==Relationship::parent){
                cout<<"Jhon has a child called "<<second.name<<endl;
            }
        }
    }
};

int main(){
    Person parent{"Jhon"};
    Person child1{"Chris"}, child2{"Matt"};
    Relationships relationships;
    relationships.add_parent_and_child(parent,child1);
    relationships.add_parent_and_child(parent,child2);
    //Research myResearch(relationships);
    // The solution is introducing an abstraction or moving it into a low level module
    BetterResearch betterReserach (relationships);
    // If low level module changes ex relations is made private
    // we can either move funtionality to a low level module or introduce an abstraction-RelationshipBrowser
    return 0;;
}

