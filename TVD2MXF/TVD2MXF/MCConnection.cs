using System;
using System.Collections.Generic;
using System.Text;

using System.Security.Cryptography;
using Microsoft.Win32;

using Microsoft.MediaCenter.Store;

namespace TVD2MXF {
  class MCConnection {

    private string pass;
    private ObjectStore store;
    private string user;
    private bool useDefaultSingleton;

    public MCConnection() {    
      this.user = "Anonymous!User";
      this.pass = this.GetProviderPass();
      this.GetObjectStore(this.useDefaultSingleton);
    }

    public ObjectStore Store {
      get {
        if (this.store == null) {
          this.GetObjectStore(this.useDefaultSingleton);
        }
        return this.store;
      }
    }

    //public void Close() {
    
    private ObjectStore GetObjectStore(bool defaultSingleton) {
      useDefaultSingleton = defaultSingleton;
      try {
        if (defaultSingleton) {
          this.store = ObjectStore.DefaultSingleton;
        } else {
          this.store = ObjectStore.Open(this.user, this.pass);
        }
      } catch (Exception exception) {
        throw new InvalidOperationException("Error : Unable to get ObjectStore." + exception.Message);
      }
      if (this.store == null) {
        throw new InvalidOperationException("ObjectStore is Null.");
      }
      return this.store;
    }

    private string GetProviderPass() {
      SHA256Managed managed = new SHA256Managed();
      byte[] bytes = Encoding.Unicode.GetBytes(this.GetEpgID());
      return Convert.ToBase64String(managed.ComputeHash(bytes));
    }

    private string GetEpgID() {
      string str = null;
      RegistryKey localMachine = Registry.LocalMachine;
      //RegistryKey localMachine = RegistryKey.OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, RegistryView.Registry64);
      if (localMachine == null) {
        throw new InvalidOperationException("Unable to access the registry.");
      }
      RegistryKey key2 = localMachine.OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Media Center\Service\EPG");
      try {
        if (key2 == null) {
          throw new InvalidOperationException("Unable to get the subkey clientId root.");
        }
        str = key2.GetValue("clientid") as string;
        if (str == null) {
          throw new InvalidOperationException("Unable to get the clientId content.");
        }
      } finally {
        if (key2 != null) {
          key2.Close();
        }
      }
      return str;
    }
  }
}
