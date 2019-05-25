using System;
using System.Collections;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;

public class AppStart {
    static void Main(string[] args) {
        FBXFileFinder exporter = new FBXFileFinder();
        exporter.ExportFBXFiles();
        Console.ReadLine();
    }
}

public class FBXFileFinder {
    [DllImport("FBXExporter.dll", EntryPoint = "exportFBX", CallingConvention = CallingConvention.Cdecl)]
    public static extern void exportFBX(StringBuilder str, bool exportUVs, bool exportNormals, bool exportColors);
    CaseInsensitiveComparer m_strComparer = new CaseInsensitiveComparer();

    public void ExportFBXFiles() {
        GetFBXFilePaths(".");
    }

    private void GetFBXFilePaths(string path) {
        string[] dirPaths = Directory.GetDirectories(path);
        foreach(string dirPath in dirPaths) {
            if(Directory.Exists(dirPath))
                GetFBXFilePaths(dirPath);
        }

        string[] fileEntries = Directory.GetFiles(path);
        foreach(string fileEntry in fileEntries) {
            var ext = Path.GetExtension(fileEntry);
            if(File.Exists(fileEntry) && m_strComparer.Compare(ext, ".fbx") == 0) {
                int atIndex = fileEntry.LastIndexOf('@') + 1;
                int dotIndex = fileEntry.LastIndexOf('.');
                var vertexAttribsToExport = fileEntry.Substring(atIndex, dotIndex - atIndex);

                bool exportUVs = vertexAttribsToExport.Contains("u") || vertexAttribsToExport.Contains("U");
                bool exportNormals = vertexAttribsToExport.Contains("n") || vertexAttribsToExport.Contains("N");
                bool exportColors = vertexAttribsToExport.Contains("c") || vertexAttribsToExport.Contains("C");

                StringBuilder stringBuilder = new StringBuilder(fileEntry);
                exportFBX(stringBuilder, exportUVs, exportNormals, exportColors);
            }
        }
    }
}
