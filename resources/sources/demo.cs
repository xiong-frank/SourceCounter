using System;
namespace HelloWorldApplication
{
    class HelloWorld
    {
        static void Main(string[] args /* args */)
        {
            // comment
            Console.WriteLine(@"Hello @""
            /*"" Not comment ""*/
            "" /*
            
            World!");
            
            /* @" */ Console.WriteLine(@"; //");
            /* @" */ Console.WriteLine("; //");
            
            Console.ReadKey();
        }
    }
}
