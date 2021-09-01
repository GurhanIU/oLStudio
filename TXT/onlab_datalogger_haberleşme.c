 public static class PCK
{
	public const byte SOP1 = 0xAA;
	public const byte EOP = 0x55;
	public const int RX_MAX_LEN = 255;

	public enum CMD_IDs
	{
		ID_WRITE_MEM,
		ID_WATCH_CONF,
		ID_WATCH_VARS,
		ID_CONNECT,
		ID_COUNT
	};

	public enum RET_VALs
	{
		PCK_NOT_RDY,
		PCK_READY,
		RESP_OK,
		PCK_INV_ID,
		PCK_CHK_ERR,
		PCK_OVERFLOW,
		PCK_INV_EOP,
	};
}    

public static void SendWatchTableConfig(int period)
{
	List<byte> transmitBuffer = new List<byte>();

	transmitBuffer.Add(SOP1);
	transmitBuffer.Add((byte)CMD_IDs.ID_WATCH_CONF);
	transmitBuffer.Add((byte)(DataBase.WatchDataBase.Count * 5 + 1));     //data length
	transmitBuffer.Add((byte)DataBase.WatchDataBase.Count);

	foreach (Var_t var in DataBase.WatchDataBase)
	{
		transmitBuffer.Add((byte)var.addr);         //var addr....
		transmitBuffer.Add((byte)(var.addr >> 8));
		transmitBuffer.Add((byte)(var.addr >> 16));
		transmitBuffer.Add((byte)(var.addr >> 24));

		transmitBuffer.Add(var.size);   //var size
	}

	transmitBuffer.Add(SerialPortController.calcChk(transmitBuffer));     //
	transmitBuffer.Add(EOP);
	//TODO: başka port seçince burada tıkanıyor?

	SerialPortController.TrySerialWrite(transmitBuffer);  //periodun 5 katına eşit timeout
	//TODO: yazma işlemi başarısız ise scope ekranı da kapatılmalı mı?
}

 public static bool TrySerialWrite(List<byte> buffer)
{
	try
	{
		/* yazmayi dene*/
		byte[] bArray = buffer.ToArray();
		HostSerialPort.Write(bArray, 0, buffer.Count);
		OnLogger.Diagnose_GetInstance().UpdateTextBox(bArray, true);

		//todo: hemen true dönme, yanıt gelince dön
		return true;
	}
	catch (Exception ex)
	{
		//write error
		MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
		WriteFailedEvent("Scope_WriteFailed", null);

		/* port açıldığı için true dön, fakat yazma işlemi başarısız olduğu için event fırlat. */
		return false;   //todo: false yaptım, neden true yaptıkydık? yazma işlemi için event'a gerek var mı?
	}
}

   public static RET_VALs RxDecoder()
{
	if(rxBuffer.Count() > 0)
	{
		rxBuffer.TryDequeue(out byte b);
		testBuf.Add(b);

		switch (rxStep)
		{
			/* Paket baslangicindaki byte1 */
			case rxSteps_t.RxWaitSOP1:

				if (b == SOP1)
				{
					/* checksum sifirla */
					calculatedChecksum = 0;
					/* paket basladi */
					rxStep = rxSteps_t.RxWaitId;
				}
				break;

			/* Paket komut id'si (ne paketi oldugunu belirtir) */
			case rxSteps_t.RxWaitId: 

				/* tanimli olmayan bir id geldi ise iptal et */
				if (b >= (byte)CMD_IDs.ID_COUNT)
				{
					/* baslangic komutuna gec */
					rxStep = rxSteps_t.RxWaitSOP1;
					//return PCK_INV_ID;
				}

				/* id degerini bufferla. */
				CmdId = b;
				/* sonraki komuta gec */
				rxStep = rxSteps_t.RxWaitLength;
				break;

			/* Paketteki veri boyutunu gosteren byte */
			case rxSteps_t.RxWaitLength: 

				/* buffer boyutunu asiyor ise iptal et*/
				if (b > RX_MAX_LEN)
				{
					/* baslangic komutuna gec */
					rxStep = rxSteps_t.RxWaitSOP1;
					return PCK_OVERFLOW;
				}

				/* veri boyutunu 'len' degiskenine bufferla. */
				dataLength = b;
				/* data indeksini sifirla. */
				data.Clear();
				/* sonraki komuta gec */
				rxStep = rxSteps_t.RxGetData;
				break;

			/* Paket verisi */
			case rxSteps_t.RxGetData:

				/* veriler 'data' dizisine bufferla. */
				data.Add(b);
				/* checksum hesaplamaya basla. */
				calculatedChecksum += b;

				/* veri alimi tamamlandi mi? */
				if (dataLength <= data.Count)       
					rxStep = rxSteps_t.RxWaitChecksum;    /* sonraki komuta gec */

				break;

			/* Paketteki Length ve Data Byte'larinin checksum toplamini iceren byte */
			case rxSteps_t.RxWaitChecksum: 

				/* checksum uyusuyor mu? */
				if (calculatedChecksum != b)
				{
					/* baslangic komutuna gec */
					rxStep = rxSteps_t.RxWaitSOP1;
					return PCK_CHK_ERR;
				}
				/* checksum degerinde hata yoksa pakete de yaz. */
				receviedChecksum = calculatedChecksum;
				/* sonraki komuta gec */
				rxStep = rxSteps_t.RxWaitEOP;
				break;

			/* Paket sonlandirici byte */
			case rxSteps_t.RxWaitEOP:

				/* baslangic komutuna gec */
				rxStep = rxSteps_t.RxWaitSOP1;

				if (b == EOP)
				{                            
					pack_handler((CMD_IDs)CmdId);
					return PCK_READY;
				}
				else
					return PCK_INV_EOP;

			   // break;

			/* Beklenmedik durum: */
			default:
				/* baslangic komutuna gec */
				rxStep = rxSteps_t.RxWaitSOP1;
				break;
		}
	}

	return PCK_NOT_RDY;
}
		
private static void pack_handler(CMD_IDs id)
{   
	//TODO: add all pack handlers
	switch (id)
	{
/*              case CMD_IDs.ID_SCOPE_VARS:

			DataBase.UpdateVariable(data, VarType.ScopeVar);
			break;
*/
		case CMD_IDs.ID_WATCH_VARS:

			DataBase.UpdateVariable(data, VarType.WatchVar);
			break;

		case CMD_IDs.ID_WRITE_MEM:

			//OnLogger.Watch_GetInstance().UpdateWriteInfo( (RET_VALs) data[0]);                    
			break;

		case CMD_IDs.ID_CONNECT:

			//MessageBox.Show("paket geldi");
			ResponseEvent("HeartBeat", null);
			SerialPortController.RespondTimer_Refresh();
			break;

		default:

		/*    if(data[0] > (int)RET_VALs.ERR_EXISTS)
			{
				string errMsg = (RET_VALs)data[0] + " error for the " + id + " command";
				MessageBox.Show(errMsg, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}*/
			break;
	}
}

public static bool UpdateVariable(List<byte> val, VarType vType)
{
	//TODO: var hiç yok ise returnler
	int varCount = val[0];
	int sizeCount = 0;

	//todo: skop penceresini kapatınca patladı.
	for (int i = 0; i < varCount; i++)
	{

		if (WatchDataBase.Count != varCount)
			return false;

		List<byte> buf = val.GetRange(sizeCount + 1, WatchDataBase[i].size);
		
		if (WatchDataBase[i].samplesFifo.Count > 1000)
			WatchDataBase[i].samplesFifo.TryDequeue(out List<byte> dummy);

		WatchDataBase[i].samplesFifo.Enqueue(buf);
		sizeCount += WatchDataBase[i].size;
		
	}

   // OnLogger.Scope_GetInstance().GraphRefreshEvent();

	//TODO: check exception and return false       
	//TODO: function overload (tek tek update edebilmek için)
	//toplam size artsın mı?

	return true;
}