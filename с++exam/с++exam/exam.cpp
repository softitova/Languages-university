//
//#include <iostream>
//
//
//using namespace std;
//
//
//struct BaseExc {
//    BaseExc(int a) {msg = a;};
//    virtual ~BaseExc() {};
//    int msg;
//    virtual void nameOut() {cout << "BaseExc " << msg << endl;}
//};
//
//struct DerivedExc : public BaseExc {
//    DerivedExc(int a) : BaseExc(a) {};
//    virtual ~DerivedExc() {};
//    void nameOut() {cout << "DerivedExc " << msg << endl;}
//};
//
//struct OneMoreExc : public BaseExc {
//    OneMoreExc(int a) : BaseExc(a) {};
//    virtual ~OneMoreExc() {};
//    void nameOut() {cout << "OneMoreExc " << msg << endl;};
//};
////
////int main() {
////    try {
////        try {
////            throw DerivedExc(5);
////        } catch (BaseExc &a) {
////            a.nameOut();
////            throw a;
////        }
////    }
////    catch (BaseExc &a)
////    {
////        a.nameOut();
////    }
////}
////int main() {
////    try {
////        throw 5;
////           } catch(int a) {
////               std::cout << a<<"\n";
////    }
////}
//
//
//int main() {
//    try {
//        try {
//            throw DerivedExc(5);
//        } catch (BaseExc &a) {
//            a.nameOut();
//            throw ;
//        }
//    }
//    catch (BaseExc &a)
//    {
//        a.nameOut();
//    }
//}
