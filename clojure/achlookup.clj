(assembly-load-from "..\\packages\\SqlServerCompact.4.0.8482.1\\lib\\System.Data.SqlServerCe.dll")

(ns achlookup
	(:gen-class))

(use '[clojure.clr.io])

(def database-file-path "c:\\code\\achlookup\\db.sdf")

(defstruct bank-record :routing-number :name)
(def routing-number (accessor bank-record :routing-number))
(def bank-name (accessor bank-record :name))

(defn print-record [record]
	(println (routing-number record) " - " (bank-name record)))

(def connection-string (str "Data Source=" database-file-path))

(defn create-database []
	(def conn (new System.Data.SqlServerCe.SqlCeEngine connection-string))
	(.CreateDatabase conn)
	(.Dispose conn))

(defn create-table []
	(def conn (new System.Data.SqlServerCe.SqlCeConnection connection-string))
	(.Open conn)
	(def command (.CreateCommand conn))
	(.set_CommandText command "CREATE TABLE Banks (RoutingNumber nvarchar(10), Name nvarchar(255))")
	(.ExecuteNonQuery command)
	(.Close conn))

(defn clear-database []
	(when (. System.IO.File Exists database-file-path)
		(. System.IO.File Delete database-file-path)))		
	
(defn insert-record [command record]
	(def parameters (.get_Parameters command))
	(.set_Value (.get_Item parameters "@RoutingNumber") (routing-number record))
	(.set_Value (.get_Item parameters "@BankName") (bank-name record))
	(.ExecuteNonQuery command))

(defn process-record [line command]
	(def record (struct bank-record 
					(subs line 0 9) 
					(subs line 27 63)))
	(print-record record)
	(insert-record command record))

(defn process-file []
	(def filepath "c:/code/achlookup/fpddir.txt")
	(let [file filepath]

		(with-open [rdr (text-reader file)]
			(def conn (new System.Data.SqlServerCe.SqlCeConnection connection-string))
			(def command (.CreateCommand conn))
			(def parameters (.get_Parameters command))
			(.Add parameters "@RoutingNumber" System.Data.SqlDbType/NVarChar)
			(.Add parameters "@BankName" System.Data.SqlDbType/NVarChar)
			(def sql "INSERT INTO Banks(RoutingNumber,Name) VALUES(@RoutingNumber,@BankName)")
			(.set_CommandText command sql)
			(.Open conn)
			(doseq [line (line-seq rdr)] (process-record line command))))
			(.Close conn))

(defn -main [& args]
	"reads the file and sticks it into a database"
	(clear-database)
	(create-database)
	(create-table)
	(process-file))
