Param (
	[Parameter(Mandatory=$True)]
	[string]$crashFile,
	[Parameter(Mandatory=$True)]
	[string]$executable
);

if (-not ("GetDbgInfo" -as [type])) {
Add-Type -typedef @"
	using System;
	using System.Collections.Generic;
	using System.Linq;
	using System.Text;
	using System.Threading.Tasks;
	using System.Runtime.InteropServices;
	using System.Threading;
	
	public class GetDbgInfo
	{
		[StructLayout(LayoutKind.Sequential)]
		public struct IMAGEHELP_SYMBOL64
		{
			public int SizeOfStuct;
			public long Address;
			public int Size;
			public int Flags;
			public int MaxNameLength;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
			public string Name;
		};
	
		[StructLayout(LayoutKind.Sequential)]
		public struct IMAGEHELP_LINEW64
		{
			public int SizeOfStruct;
			public IntPtr Key;
			public uint LineNumber;
			public IntPtr FileName;
			public ulong Address;
		};
	
		[DllImport("dbghelp.dll", SetLastError=true)]
		private static extern ulong SymLoadModule64(IntPtr hProcess, IntPtr hFile, string ImageName, string ModuleName, ulong BaseOfDll, uint SizeOfDll);
		[DllImport("dbghelp.dll")]
		private static extern bool SymInitializeW(IntPtr hProcess, string userSearchPath, bool InvadeProcess);
		[DllImport("dbghelp.dll")]
		private static extern ulong SymCleanup(IntPtr hProcess);
		[DllImport("dbghelp.dll")]
		private static extern bool SymGetSymFromAddr64(IntPtr hProcess, ulong addr, ref long outOffset, ref IMAGEHELP_SYMBOL64 symbol);
		[DllImport("dbghelp.dll")]
		private static extern bool SymGetLineFromAddrW64(IntPtr hProcess, ulong addr, ref long outOffset, ref IMAGEHELP_LINEW64 symbol);
	
	
		static IMAGEHELP_SYMBOL64 CreateSymbol()
		{
			IMAGEHELP_SYMBOL64 smb = new IMAGEHELP_SYMBOL64();
			smb.Address = 0;
			smb.Flags = 0;
			smb.MaxNameLength = 254;
			smb.Size = 0;
			smb.SizeOfStuct = Marshal.SizeOf(smb) - 254*2;
			return (smb);
		}
	
		static IMAGEHELP_LINEW64 CreateLine()
		{
			IMAGEHELP_LINEW64 smb = new IMAGEHELP_LINEW64();
			smb.Address = 0;
			smb.LineNumber = 0;
			smb.SizeOfStruct = Marshal.SizeOf(smb);
			return (smb);
		}
	
		public GetDbgInfo(string module, string baseAddrString)
		{
			ulong baseAddr = Convert.ToUInt64(baseAddrString, 16);
			m_Ptr = new IntPtr(Interlocked.Increment(ref lNum));
			ulong ret = 0;
			if (!SymInitializeW(m_Ptr, null, false))
			{
				throw (new Exception());
			}
			var v = SymLoadModule64(m_Ptr, System.IntPtr.Zero, module, string.Empty, baseAddr, 0);
			if(v == 0)
			{
				System.Console.WriteLine(ret);
				throw (new Exception());
			}
		}
	
		~GetDbgInfo() {
			if(m_Ptr != null) {
				SymCleanup(m_Ptr);
			}
		}
		
		public string GetSymbolFromAddr(string addrString)
		{
			ulong addr = Convert.ToUInt64(addrString, 16);
			IMAGEHELP_SYMBOL64 smb = CreateSymbol();
			IMAGEHELP_LINEW64 ln = CreateLine();
			long offset = 0;
			if (!SymGetSymFromAddr64(m_Ptr, addr, ref offset, ref smb))
			{
				return ("? : ? : 0");
			}
			if (!SymGetLineFromAddrW64(m_Ptr, addr, ref offset, ref ln))
			{
				return ("? : ? : 0");
			}
			string s = Marshal.PtrToStringUni(ln.FileName);
			return (smb.Name + " : " + s + " : " + ln.LineNumber);
		}
		IntPtr m_Ptr = new IntPtr(0x01);
		static long lNum = 0x01;
	}
"@
}



$firstRun = $true
$y = $null
Get-Content $crashFile | %{
	if($firstRun -eq $true) {
		$firstRun = $false;
		$y = New-Object GetDbgInfo($executable, $_) 
	} else {
		Write-Host $y.GetSymbolFromAddr($_)
	}
}
Remove-Variable y