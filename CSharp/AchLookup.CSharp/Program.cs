using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlServerCe;
using System.IO;
using System.Linq;
using System.Text;

namespace AchLookup.CSharp
{
    struct BankRecord
    {
        public string RoutingNumber;
        public string Name;
    }

    class Program
    {
        static void Main(string[] args)
        {
            var databaseFilePath = @"c:\code\achlookup\db.sdf";
            var connectionString = String.Format("Data Source={0}", databaseFilePath);
            var filePath = @"c:\code\achlookup\fpddir.txt";

            if(File.Exists(databaseFilePath))
            {
                File.Delete(databaseFilePath);
            }

            var engine = new SqlCeEngine(connectionString);
            engine.CreateDatabase();
            engine.Dispose();

            using(var connection = new SqlCeConnection(connectionString))
            {
                connection.Open();
                var command = connection.CreateCommand();
                command.CommandText = @"
CREATE TABLE Banks (RoutingNumber nvarchar(10), Name nvarchar(255))
";
                command.ExecuteNonQuery();
                connection.Close();
            }

            var sql = @"
INSERT INTO Banks(RoutingNumber,Name) VALUES(@RoutingNumber,@BankName)
";

            using (var connection = new SqlCeConnection(connectionString))
            {
                connection.Open();
                var command = connection.CreateCommand();
                command.CommandText = sql;
                command.Parameters.Add("@RoutingNumber", SqlDbType.NVarChar);
                command.Parameters.Add("@BankName", SqlDbType.NVarChar);

                foreach (var line in File.ReadLines(filePath))
                {
                    BankRecord record;
                    record.RoutingNumber = line.Substring(0, 9);
                    record.Name = line.Substring(27, 36);

                    command.Parameters["@RoutingNumber"].Value = record.RoutingNumber;
                    command.Parameters["@BankName"].Value = record.Name;

                    command.ExecuteNonQuery();
                }
            }


            Console.WriteLine("All DONE :)");
            Console.ReadLine();
        }
    }
}
