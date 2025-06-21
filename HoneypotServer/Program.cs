using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

class Program
{
    private static readonly string honeypotFolder = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "honeypot_files");
    private static readonly string logFile = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "honeypot_log.txt");
    private const int PORT = 9090;

    static void Main()
    {
        Directory.CreateDirectory(honeypotFolder);

        TcpListener listener = new TcpListener(IPAddress.Any, PORT);
        listener.Start();
        Console.WriteLine($"Honeypot server running on port {PORT}");

        while (true)
        {
            var client = listener.AcceptTcpClient();
            Task.Run(() => HandleClient(client));
        }
    }

    static void HandleClient(TcpClient client)
    {
        string clientIp = client.Client.RemoteEndPoint?.ToString() ?? "Unknown";
        Console.WriteLine($"Client connected: {clientIp}");

        using NetworkStream stream = client.GetStream();
        using StreamReader reader = new StreamReader(stream);
        using StreamWriter writer = new StreamWriter(stream) { AutoFlush = true };

        try
        {
            writer.WriteLine("Welcome to RCM Honeypot Server");
            writer.WriteLine("Enter file name to download:");

            string? filename = reader.ReadLine();

            if (string.IsNullOrWhiteSpace(filename))
            {
                writer.WriteLine("No filename entered.");
                return;
            }

            LogRequest(clientIp, filename);

            string filePath = Path.Combine(honeypotFolder, filename);
            if (File.Exists(filePath))
            {
                string content = File.ReadAllText(filePath);
                writer.WriteLine("--- FILE CONTENT START ---");
                writer.WriteLine(content);
                writer.WriteLine("--- FILE CONTENT END ---");
            }
            else
            {
                writer.WriteLine("File not found.");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error handling client {clientIp}: {ex.Message}");
        }

        client.Close();
        Console.WriteLine($"Client disconnected: {clientIp}");
    }

    static void LogRequest(string clientIp, string file)
    {
        if (string.IsNullOrEmpty(clientIp) || string.IsNullOrEmpty(file))
            return;

        string log = $"[{DateTime.Now}] IP: {clientIp} requested: {file}";
        File.AppendAllText(logFile, log + Environment.NewLine);
    }
}
