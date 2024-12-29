using Sodium;

namespace Sunlight.ManagementStudio.Models.Controller.Tcp;

public class SunlightTcpCipher
{
    private byte[] _key = new byte[32];

    private byte[] _encryptNonce = new byte[12];
    private byte[] _decryptNonce = new byte[12];

    public SunlightTcpCipher(byte[] key)
    {
        Array.Copy(key, _key, Math.Min(key.Length, _key.Length));
    }

    public void Encrypt(byte[] additionalData, byte[] bytes, out byte[] outBytes)
    {
        Increase(_encryptNonce);

        outBytes = SecretAeadChaCha20Poly1305IETF.Encrypt(bytes, _encryptNonce, _key, additionalData);
    }

    public void Decrypt(byte[] additionalData, byte[] bytes, out byte[] outBytes)
    {
        Increase(_decryptNonce);

        outBytes = SecretAeadChaCha20Poly1305IETF.Decrypt(bytes, _decryptNonce, _key, additionalData);
    }

    private void Increase(byte[] nonce)
    {
        for (int i = nonce.Length - 1; i >= 0; i--)
        {
            if (++nonce[i] != 0)
            {
                break;
            }
        }
    }
}
